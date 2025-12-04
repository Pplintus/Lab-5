#include "DataManager.h"
#include <fstream>

using namespace SalaryDepartment;
using namespace System::IO;

// Функции сравнения для сортировки
int CompareEmployeesByName(Employee^ a, Employee^ b) {
    return String::Compare(a->FullName, b->FullName);
}

int CompareEmployeesBySalary(Employee^ a, Employee^ b) {
    return b->GetSalary().CompareTo(a->GetSalary());
}

int CompareWorkTypesByName(WorkType^ a, WorkType^ b) {
    return String::Compare(a->Name, b->Name);
}

DataManager::DataManager() {
    employees = gcnew Dictionary<String^, Employee^>();
    workTypes = gcnew Dictionary<String^, WorkType^>();
}

bool DataManager::AddWorkType(String^ name, int salary) {
    try {
        // Валидация входных данных
        if (System::String::IsNullOrWhiteSpace(name)) {
            throw gcnew System::ArgumentException("Название должности не может быть пустым");
        }

        String^ trimmedName = name->Trim();
        if (trimmedName->Length < 2) {
            throw gcnew System::ArgumentException("Название должности должно содержать минимум 2 символа");
        }

        if (trimmedName->Length > 100) {
            throw gcnew System::ArgumentException("Название должности не может быть длиннее 100 символов");
        }

        // Проверка на специальные символы
        array<Char>^ invalidChars = { '/', '\\', ':', '*', '?', '"', '<', '>', '|', '@', '#', '$', '%', '^', '&', '=', '+' };
        for each (Char c in invalidChars) {
            if (trimmedName->Contains(c.ToString())) {
                throw gcnew System::ArgumentException("Название должности содержит запрещенные символы");
            }
        }

        if (salary <= 0) {
            throw gcnew System::ArgumentException("Оклад должен быть положительным числом");
        }

        if (salary < 10000) {
            throw gcnew System::ArgumentException("Оклад не может быть меньше 10,000 руб.");
        }

        if (salary > 1000000) {
            throw gcnew System::ArgumentException("Оклад не может превышать 1,000,000 руб.");
        }

        if (workTypes->ContainsKey(trimmedName)) {
            throw gcnew System::ArgumentException("Должность с таким названием уже существует");
        }

        workTypes->Add(trimmedName, gcnew WorkType(trimmedName, salary));
        return true;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка добавления должности: " + ex->Message);
        throw; // Пробрасываем дальше
    }
}

bool DataManager::UpdateWorkType(String^ oldName, String^ newName, int salary) {
    try {
        // Валидация нового имени
        if (System::String::IsNullOrWhiteSpace(newName)) {
            throw gcnew System::ArgumentException("Название должности не может быть пустым");
        }

        String^ trimmedNewName = newName->Trim();
        if (trimmedNewName->Length < 2) {
            throw gcnew System::ArgumentException("Название должности должно содержать минимум 2 символа");
        }

        if (trimmedNewName->Length > 100) {
            throw gcnew System::ArgumentException("Название должности не может быть длиннее 100 символов");
        }

        if (salary <= 0)
            return false;

        if (!workTypes->ContainsKey(oldName))
            return false;

        // Проверяем, используется ли должность сотрудниками
        bool isUsed = false;
        for each (KeyValuePair<String^, Employee^> emp in employees) {
            if (emp.Value->Position->Name == oldName) {
                isUsed = true;
                break;
            }
        }

        // Если должность используется, обновляем ее без удаления
        if (isUsed) {
            if (oldName != newName) {
                // Нельзя менять имя используемой должности
                throw gcnew System::ArgumentException("Нельзя изменить название должности, которая используется сотрудниками");
            }
            // Обновляем только зарплату
            workTypes[oldName]->Salary = salary;

            // Обновляем зарплату у всех сотрудников с этой должностью
            for each (KeyValuePair<String^, Employee^> emp in employees) {
                if (emp.Value->Position->Name == oldName) {
                    emp.Value->Position->Salary = salary;
                }
            }
        }
        else {
            // Должность не используется, можно удалить и создать новую
            workTypes->Remove(oldName);
            workTypes->Add(newName, gcnew WorkType(newName, salary));
        }

        return true;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка обновления должности: " + ex->Message);
        throw;
    }
}

bool DataManager::DeleteWorkType(String^ name) {
    if (!workTypes->ContainsKey(name))
        return false;

    // Проверяем, используется ли должность
    for each (KeyValuePair<String^, Employee^> emp in employees) {
        if (emp.Value->Position->Name == name)
            return false;
    }

    workTypes->Remove(name);
    return true;
}

bool DataManager::AddEmployee(String^ name, String^ positionName, int workDays) {
    try {
        // Валидация имени
        if (System::String::IsNullOrWhiteSpace(name)) {
            throw gcnew System::ArgumentException("Имя сотрудника не может быть пустым");
        }

        String^ trimmedName = name->Trim();
        if (trimmedName->Length < 2) {
            throw gcnew System::ArgumentException("Имя сотрудника должно содержать минимум 2 символа");
        }

        if (trimmedName->Length > 50) {
            throw gcnew System::ArgumentException("Имя сотрудника не может быть длиннее 50 символов");
        }

        // Проверка на специальные символы
        array<Char>^ invalidChars = { '/', '\\', ':', '*', '?', '"', '<', '>', '|', '@', '#', '$', '%', '^', '&', '=', '+', ')', '(' };
        for each (Char c in invalidChars) {
            if (trimmedName->Contains(c.ToString())) {
                throw gcnew System::ArgumentException("Имя сотрудника содержит запрещенные символы");
            }
        }

        if (employees->ContainsKey(trimmedName)) {
            throw gcnew System::ArgumentException("Сотрудник с таким именем уже существует");
        }

        if (!workTypes->ContainsKey(positionName)) {
            throw gcnew System::ArgumentException("Указанная должность не существует");
        }

        if (workDays <= 0 || workDays > 30) {
            throw gcnew System::ArgumentException("Количество рабочих дней должно быть от 1 до 30");
        }

        Employee^ emp = gcnew Employee(trimmedName, workTypes[positionName], workDays);
        employees->Add(trimmedName, emp);
        return true;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка добавления сотрудника: " + ex->Message);
        throw;
    }
}

bool DataManager::UpdateEmployee(String^ oldName, String^ newName, String^ positionName, int workDays) {
    try {
        // Валидация нового имени
        if (System::String::IsNullOrWhiteSpace(newName)) {
            throw gcnew System::ArgumentException("Имя сотрудника не может быть пустым");
        }

        String^ trimmedNewName = newName->Trim();
        if (trimmedNewName->Length < 2) {
            throw gcnew System::ArgumentException("Имя сотрудника должно содержать минимум 2 символа");
        }

        if (trimmedNewName->Length > 100) {
            throw gcnew System::ArgumentException("Имя сотрудника не может быть длиннее 100 символов");
        }

        if (!workTypes->ContainsKey(positionName)) {
            throw gcnew System::ArgumentException("Указанная должность не существует");
        }

        if (workDays <= 0 || workDays > 30) {
            throw gcnew System::ArgumentException("Количество рабочих дней должно быть от 1 до 30");
        }

        if (!employees->ContainsKey(oldName))
            return false;

        if (oldName != newName && employees->ContainsKey(newName))
            return false;

        Employee^ emp = employees[oldName];
        if (oldName != newName) {
            employees->Remove(oldName);
            employees->Add(newName, emp);
            emp->FullName = newName;
        }

        emp->Position = workTypes[positionName];
        emp->WorkDays = workDays;
        return true;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка обновления сотрудника: " + ex->Message);
        throw;
    }
}

bool DataManager::DeleteEmployee(String^ name) {
    if (!employees->ContainsKey(name))
        return false;

    employees->Remove(name);
    return true;
}

double DataManager::CalculateTotalSalary() {
    double total = 0;
    for each (KeyValuePair<String^, Employee^> emp in employees) {
        total += emp.Value->GetSalary();
    }
    return total;
}

bool DataManager::SaveToFiles(String^ employeesFile, String^ workTypesFile) {
    try {
        // Сохраняем должности
        StreamWriter^ sw1 = gcnew StreamWriter(workTypesFile, false);
        for each (KeyValuePair<String^, WorkType^> wt in workTypes) {
            sw1->WriteLine("{0},{1}", wt.Value->Name, wt.Value->Salary);
        }
        sw1->Close();

        // Сохраняем сотрудников
        StreamWriter^ sw2 = gcnew StreamWriter(employeesFile, false);
        for each (KeyValuePair<String^, Employee^> emp in employees) {
            sw2->WriteLine("{0},{1},{2}",
                emp.Value->FullName,
                emp.Value->Position->Name,
                emp.Value->WorkDays);
        }
        sw2->Close();
        return true;
    }
    catch (Exception^) {
        return false;
    }
}

bool DataManager::LoadFromFiles(String^ employeesFile, String^ workTypesFile) {
    try {
        employees->Clear();
        workTypes->Clear();

        // Загружаем должности
        if (File::Exists(workTypesFile)) {
            array<String^>^ lines = File::ReadAllLines(workTypesFile);
            for each (String ^ line in lines) {
                if (String::IsNullOrWhiteSpace(line)) continue;

                array<String^>^ parts = line->Split(',');
                if (parts->Length >= 2) {
                    String^ name = parts[0];
                    int salary = Convert::ToInt32(parts[1]);
                    AddWorkType(name, salary);
                }
            }
        }

        // Загружаем сотрудников
        if (File::Exists(employeesFile)) {
            array<String^>^ lines = File::ReadAllLines(employeesFile);
            for each (String ^ line in lines) {
                if (String::IsNullOrWhiteSpace(line)) continue;

                array<String^>^ parts = line->Split(',');
                if (parts->Length >= 3) {
                    String^ name = parts[0];
                    String^ position = parts[1];
                    int days = Convert::ToInt32(parts[2]);
                    AddEmployee(name, position, days);
                }
            }
        }
        return true;
    }
    catch (Exception^) {
        return false;
    }
}

List<Employee^>^ DataManager::GetEmployeesSortedByName() {
    List<Employee^>^ result = gcnew List<Employee^>();
    for each (KeyValuePair<String^, Employee^> emp in employees) {
        result->Add(emp.Value);
    }

    Comparison<Employee^>^ comparer = gcnew Comparison<Employee^>(CompareEmployeesByName);
    result->Sort(comparer);
    return result;
}

List<Employee^>^ DataManager::GetEmployeesSortedBySalary() {
    List<Employee^>^ result = gcnew List<Employee^>();
    for each (KeyValuePair<String^, Employee^> emp in employees) {
        result->Add(emp.Value);
    }

    Comparison<Employee^>^ comparer = gcnew Comparison<Employee^>(CompareEmployeesBySalary);
    result->Sort(comparer);
    return result;
}

List<WorkType^>^ DataManager::GetWorkTypesSortedByName() {
    List<WorkType^>^ result = gcnew List<WorkType^>();
    for each (KeyValuePair<String^, WorkType^> wt in workTypes) {
        result->Add(wt.Value);
    }

    Comparison<WorkType^>^ comparer = gcnew Comparison<WorkType^>(CompareWorkTypesByName);
    result->Sort(comparer);
    return result;
}