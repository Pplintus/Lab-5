#include "TextFileManager.h"
#include "Employee.h"
#include "WorkType.h"

using namespace SalaryDepartment;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;

TextFileManager::TextFileManager() {
    // Конструктор
}

bool TextFileManager::SaveEmployeesToFile(Dictionary<String^, Employee^>^ employees, String^ filePath) {
    try {
        // Создаем StreamWriter для записи в файл
        StreamWriter^ writer = gcnew StreamWriter(filePath, false, Encoding::UTF8);

        // Записываем заголовок
        writer->WriteLine("ФИО;Должность;Отработано дней");

        // Записываем данные каждого сотрудника
        for each (KeyValuePair<String^, Employee^> pair in employees) {
            Employee^ emp = pair.Value;

            // Экранируем точку с запятой в данных
            String^ safeName = emp->FullName->Replace(";", ",");
            String^ safePosition = emp->Position->Name->Replace(";", ",");

            writer->WriteLine(String::Format("{0};{1};{2}",
                safeName,
                safePosition,
                emp->WorkDays));
        }

        writer->Close();
        return true;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка сохранения сотрудников: " + ex->Message);
        return false;
    }
}

bool TextFileManager::LoadEmployeesFromFile(Dictionary<String^, Employee^>^% employees,
    Dictionary<String^, WorkType^>^ workTypes,
    String^ filePath) {
    try {
        if (!File::Exists(filePath)) {
            System::Diagnostics::Debug::WriteLine("Файл не найден: " + filePath);
            return false;
        }

        StreamReader^ reader = gcnew StreamReader(filePath, Encoding::UTF8);

        // Читаем заголовок
        String^ header = reader->ReadLine();
        if (header == nullptr || !header->Contains(";")) {
            reader->Close();
            System::Diagnostics::Debug::WriteLine("Неверный формат файла: отсутствует заголовок");
            return false;
        }

        employees->Clear();
        int loadedCount = 0;
        int errorCount = 0;

        // Читаем данные
        while (!reader->EndOfStream) {
            String^ line = reader->ReadLine();
            if (String::IsNullOrWhiteSpace(line)) {
                continue;
            }

            array<String^>^ parts = line->Split(';');
            if (parts->Length < 3) {
                errorCount++;
                System::Diagnostics::Debug::WriteLine(
                    String::Format("Ошибка в строке: неверный формат - '{0}'", line));
                continue;
            }

            String^ name = parts[0]->Trim();
            String^ positionName = parts[1]->Trim();
            String^ workDaysStr = parts[2]->Trim();

            // Проверяем обязательные поля
            if (String::IsNullOrEmpty(name) ||
                String::IsNullOrEmpty(positionName) ||
                String::IsNullOrEmpty(workDaysStr)) {
                errorCount++;
                continue;
            }

            // Проверяем существование должности
            if (!workTypes->ContainsKey(positionName)) {
                System::Diagnostics::Debug::WriteLine(
                    String::Format("Должность не найдена: '{0}' для сотрудника '{1}'",
                        positionName, name));
                errorCount++;
                continue;
            }

            // Преобразуем количество дней
            int workDays;
            if (!Int32::TryParse(workDaysStr, workDays)) {
                errorCount++;
                continue;
            }

            if (workDays < 1 || workDays > 30) {
                errorCount++;
                continue;
            }

            // Создаем сотрудника
            WorkType^ wt = workTypes[positionName];
            Employee^ emp = gcnew Employee(name, wt, workDays);

            // Добавляем в словарь
            if (!employees->ContainsKey(name)) {
                employees->Add(name, emp);
                loadedCount++;
            }
        }

        reader->Close();
        System::Diagnostics::Debug::WriteLine(
            String::Format("Загружено сотрудников: {0}, ошибок: {1}", loadedCount, errorCount));

        return loadedCount > 0;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка загрузки сотрудников: " + ex->Message);
        return false;
    }
}

bool TextFileManager::SaveWorkTypesToFile(Dictionary<String^, WorkType^>^ workTypes, String^ filePath) {
    try {
        StreamWriter^ writer = gcnew StreamWriter(filePath, false, Encoding::UTF8);

        // Записываем заголовок
        writer->WriteLine("Название должности;Оклад");

        // Записываем данные каждой должности
        for each (KeyValuePair<String^, WorkType^> pair in workTypes) {
            WorkType^ wt = pair.Value;

            // Экранируем точку с запятой в данных
            String^ safeName = wt->Name->Replace(";", ",");

            writer->WriteLine(String::Format("{0};{1}",
                safeName,
                wt->Salary));
        }

        writer->Close();
        return true;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка сохранения должностей: " + ex->Message);
        return false;
    }
}

bool TextFileManager::LoadWorkTypesFromFile(Dictionary<String^, WorkType^>^% workTypes, String^ filePath) {
    try {
        if (!File::Exists(filePath)) {
            System::Diagnostics::Debug::WriteLine("Файл не найден: " + filePath);
            return false;
        }

        StreamReader^ reader = gcnew StreamReader(filePath, Encoding::UTF8);

        // Читаем заголовок
        String^ header = reader->ReadLine();
        if (header == nullptr || !header->Contains(";")) {
            reader->Close();
            System::Diagnostics::Debug::WriteLine("Неверный формат файла: отсутствует заголовок");
            return false;
        }

        workTypes->Clear();
        int loadedCount = 0;
        int errorCount = 0;

        // Читаем данные
        while (!reader->EndOfStream) {
            String^ line = reader->ReadLine();
            if (String::IsNullOrWhiteSpace(line)) {
                continue;
            }

            array<String^>^ parts = line->Split(';');
            if (parts->Length < 2) {
                errorCount++;
                continue;
            }

            String^ name = parts[0]->Trim();
            String^ salaryStr = parts[1]->Trim();

            // Проверяем обязательные поля
            if (String::IsNullOrEmpty(name) || String::IsNullOrEmpty(salaryStr)) {
                errorCount++;
                continue;
            }

            // Преобразуем оклад
            int salary;
            if (!Int32::TryParse(salaryStr, salary)) {
                errorCount++;
                continue;
            }

            if (salary < 10000 || salary > 1000000) {
                errorCount++;
                continue;
            }

            // Создаем должность
            WorkType^ wt = gcnew WorkType(name, salary);

            // Добавляем в словарь
            if (!workTypes->ContainsKey(name)) {
                workTypes->Add(name, wt);
                loadedCount++;
            }
        }

        reader->Close();
        System::Diagnostics::Debug::WriteLine(
            String::Format("Загружено должностей: {0}, ошибок: {1}", loadedCount, errorCount));

        return loadedCount > 0;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка загрузки должностей: " + ex->Message);
        return false;
    }
}

bool TextFileManager::SaveAllData(Dictionary<String^, Employee^>^ employees,
    Dictionary<String^, WorkType^>^ workTypes,
    String^ employeesFile,
    String^ workTypesFile) {
    try {
        // Сохраняем должности
        if (!SaveWorkTypesToFile(workTypes, workTypesFile)) {
            return false;
        }

        // Сохраняем сотрудников
        if (!SaveEmployeesToFile(employees, employeesFile)) {
            return false;
        }

        return true;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка сохранения всех данных: " + ex->Message);
        return false;
    }
}

bool TextFileManager::LoadAllData(Dictionary<String^, Employee^>^% employees,
    Dictionary<String^, WorkType^>^% workTypes,
    String^ employeesFile,
    String^ workTypesFile) {
    try {
        // Сначала загружаем должности
        if (!LoadWorkTypesFromFile(workTypes, workTypesFile)) {
            System::Diagnostics::Debug::WriteLine("Не удалось загрузить должности");
            return false;
        }

        // Затем загружаем сотрудников (им нужны должности)
        if (!LoadEmployeesFromFile(employees, workTypes, employeesFile)) {
            System::Diagnostics::Debug::WriteLine("Не удалось загрузить сотрудников");
            return false;
        }

        return true;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка загрузки всех данных: " + ex->Message);
        return false;
    }
}

bool TextFileManager::SaveAllDataToDefault(Dictionary<String^, Employee^>^ employees,
    Dictionary<String^, WorkType^>^ workTypes) {
    return SaveAllData(employees, workTypes, defaultEmployeesFile, defaultWorkTypesFile);
}

bool TextFileManager::LoadAllDataFromDefault(Dictionary<String^, Employee^>^% employees,
    Dictionary<String^, WorkType^>^% workTypes) {
    return LoadAllData(employees, workTypes, defaultEmployeesFile, defaultWorkTypesFile);
}