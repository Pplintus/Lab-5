#include "DataManager.h"
#include <sqlite3.h>
#include <msclr/marshal_cppstd.h>
#include "TextFileManager.h"

using namespace SalaryDepartment;
using namespace System::Runtime::InteropServices;
using namespace msclr::interop;
using namespace System::IO;
using namespace System::Text;

// Вспомогательная статическая функция для выполнения SQL запросов
bool DataManager::ExecuteSQLiteQuery(String^ query) {
    sqlite3* db;
    char* errMsg = 0;

    // Конвертируем строку
    marshal_context^ context = gcnew marshal_context();
    const char* sql = context->marshal_as<const char*>(query);

    // Открываем базу данных
    int rc = sqlite3_open("salary_department.db", &db);
    if (rc) {
        sqlite3_close(db);
        delete context;
        return false;
    }

    // Выполняем запрос
    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    if (rc != SQLITE_OK) {
        if (errMsg != 0) {
            sqlite3_free(errMsg);
        }
        sqlite3_close(db);
        delete context;
        return false;
    }

    sqlite3_close(db);
    delete context;
    return true;
}

// Вспомогательная статическая функция для получения данных
bool DataManager::GetSQLiteData(String^ query, List<List<String^>^>^% results) {
    sqlite3* db;
    sqlite3_stmt* stmt;

    marshal_context^ context = gcnew marshal_context();
    const char* sql = context->marshal_as<const char*>(query);

    int rc = sqlite3_open("salary_department.db", &db);
    if (rc) {
        sqlite3_close(db);
        delete context;
        return false;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        delete context;
        return false;
    }

    results->Clear();

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        List<String^>^ row = gcnew List<String^>();
        int colCount = sqlite3_column_count(stmt);

        for (int i = 0; i < colCount; i++) {
            const char* colText = (const char*)sqlite3_column_text(stmt, i);
            if (colText != nullptr) {
                row->Add(gcnew String(colText));
            }
            else {
                row->Add("");
            }
        }
        results->Add(row);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    delete context;
    return true;
}

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
    InitializeDatabase();
    LoadFromDatabase();
}

DataManager::DataManager(String^ databasePath) {
    employees = gcnew Dictionary<String^, Employee^>();
    workTypes = gcnew Dictionary<String^, WorkType^>();
    // Пока используем фиксированную базу
    InitializeDatabase();
    LoadFromDatabase();
}

bool DataManager::InitializeDatabase() {
    // Создаем таблицу для должностей, если ее нет
    String^ createWorkTypesTable =
        "CREATE TABLE IF NOT EXISTS worktypes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT UNIQUE NOT NULL, "
        "salary INTEGER NOT NULL CHECK(salary >= 10000 AND salary <= 1000000)"
        ");";

    // Создаем таблицу для сотрудников, если ее нет
    String^ createEmployeesTable =
        "CREATE TABLE IF NOT EXISTS employees ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "full_name TEXT UNIQUE NOT NULL, "
        "position_name TEXT NOT NULL, "
        "work_days INTEGER NOT NULL CHECK(work_days >= 1 AND work_days <= 30), "
        "FOREIGN KEY (position_name) REFERENCES worktypes(name) ON DELETE RESTRICT"
        ");";

    return ExecuteSQLiteQuery(createWorkTypesTable) &&
        ExecuteSQLiteQuery(createEmployeesTable);
}

void DataManager::LoadWorkTypesFromDB() {
    workTypes->Clear();

    String^ query = "SELECT name, salary FROM worktypes;";
    List<List<String^>^>^ results = gcnew List<List<String^>^>();

    if (GetSQLiteData(query, results)) {
        for each (List<String^> ^ row in results) {
            if (row->Count >= 2) {
                String^ name = row[0];
                int salary = Convert::ToInt32(row[1]);
                workTypes->Add(name, gcnew WorkType(name, salary));
            }
        }
    }
}

void DataManager::LoadEmployeesFromDB() {
    employees->Clear();

    String^ query = "SELECT e.full_name, e.position_name, e.work_days, w.salary "
        "FROM employees e "
        "JOIN worktypes w ON e.position_name = w.name;";

    List<List<String^>^>^ results = gcnew List<List<String^>^>();

    if (GetSQLiteData(query, results)) {
        for each (List<String^> ^ row in results) {
            if (row->Count >= 4) {
                String^ name = row[0];
                String^ positionName = row[1];
                int workDays = Convert::ToInt32(row[2]);
                int salary = Convert::ToInt32(row[3]);

                if (workTypes->ContainsKey(positionName)) {
                    Employee^ emp = gcnew Employee(name, workTypes[positionName], workDays);
                    employees->Add(name, emp);
                }
            }
        }
    }
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

        // Проверяем наличие в БД
        String^ checkQuery = String::Format("SELECT COUNT(*) FROM worktypes WHERE name = '{0}';", trimmedName);
        List<List<String^>^>^ results = gcnew List<List<String^>^>();

        if (GetSQLiteData(checkQuery, results) && results->Count > 0) {
            List<String^>^ firstRow = results[0];
            if (firstRow->Count > 0) {
                int count = Convert::ToInt32(firstRow[0]);
                if (count > 0) {
                    throw gcnew System::ArgumentException("Должность с таким названием уже существует");
                }
            }
        }

        // Добавляем в БД
        String^ insertQuery = String::Format(
            "INSERT INTO worktypes (name, salary) VALUES ('{0}', {1});",
            trimmedName, salary);

        if (ExecuteSQLiteQuery(insertQuery)) {
            // Обновляем кэш
            workTypes->Add(trimmedName, gcnew WorkType(trimmedName, salary));
            return true;
        }

        return false;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка добавления должности: " + ex->Message);
        throw;
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

        // Проверяем существование старой должности в БД
        String^ checkOldQuery = String::Format("SELECT COUNT(*) FROM worktypes WHERE name = '{0}';", oldName);
        List<List<String^>^>^ results = gcnew List<List<String^>^>();

        if (!GetSQLiteData(checkOldQuery, results) || results->Count == 0) {
            return false;
        }

        List<String^>^ firstRow = results[0];
        if (firstRow->Count == 0 || Convert::ToInt32(firstRow[0]) == 0) {
            return false;
        }

        // Проверяем, используется ли должность сотрудниками
        String^ checkUsedQuery = String::Format("SELECT COUNT(*) FROM employees WHERE position_name = '{0}';", oldName);
        List<List<String^>^>^ usageResults = gcnew List<List<String^>^>();

        bool isUsed = false;
        if (GetSQLiteData(checkUsedQuery, usageResults) && usageResults->Count > 0) {
            List<String^>^ usageRow = usageResults[0];
            if (usageRow->Count > 0) {
                int count = Convert::ToInt32(usageRow[0]);
                isUsed = count > 0;
            }
        }

        // Если должность используется, обновляем ее без изменения имени
        if (isUsed) {
            if (oldName != trimmedNewName) {
                // Нельзя менять имя используемой должности
                throw gcnew System::ArgumentException("Нельзя изменить название должности, которая используется сотрудниками");
            }
            // Обновляем только зарплату
            String^ updateQuery = String::Format(
                "UPDATE worktypes SET salary = {0} WHERE name = '{1}';",
                salary, oldName);

            if (ExecuteSQLiteQuery(updateQuery)) {
                // Обновляем кэш
                if (workTypes->ContainsKey(oldName)) {
                    workTypes[oldName]->Salary = salary;
                }
                return true;
            }
        }
        else {
            // Должность не используется, можно обновить
            String^ updateQuery = String::Format(
                "UPDATE worktypes SET name = '{0}', salary = {1} WHERE name = '{2}';",
                trimmedNewName, salary, oldName);

            if (ExecuteSQLiteQuery(updateQuery)) {
                // Обновляем кэш
                if (workTypes->ContainsKey(oldName)) {
                    WorkType^ wt = workTypes[oldName];
                    workTypes->Remove(oldName);
                    wt->Name = trimmedNewName;
                    wt->Salary = salary;
                    workTypes->Add(trimmedNewName, wt);
                }
                return true;
            }
        }

        return false;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка обновления должности: " + ex->Message);
        throw;
    }
}

bool DataManager::DeleteWorkType(String^ name) {
    // Проверяем существование в БД
    String^ checkQuery = String::Format("SELECT COUNT(*) FROM worktypes WHERE name = '{0}';", name);
    List<List<String^>^>^ results = gcnew List<List<String^>^>();

    if (!GetSQLiteData(checkQuery, results) || results->Count == 0) {
        return false;
    }

    List<String^>^ firstRow = results[0];
    if (firstRow->Count == 0 || Convert::ToInt32(firstRow[0]) == 0) {
        return false;
    }

    // Проверяем, используется ли должность
    String^ checkUsedQuery = String::Format("SELECT COUNT(*) FROM employees WHERE position_name = '{0}';", name);
    List<List<String^>^>^ usageResults = gcnew List<List<String^>^>();

    if (GetSQLiteData(checkUsedQuery, usageResults) && usageResults->Count > 0) {
        List<String^>^ usageRow = usageResults[0];
        if (usageRow->Count > 0) {
            int count = Convert::ToInt32(usageRow[0]);
            if (count > 0) {
                return false;
            }
        }
    }

    // Удаляем из БД
    String^ deleteQuery = String::Format("DELETE FROM worktypes WHERE name = '{0}';", name);

    if (ExecuteSQLiteQuery(deleteQuery)) {
        // Обновляем кэш
        if (workTypes->ContainsKey(name)) {
            workTypes->Remove(name);
        }
        return true;
    }

    return false;
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

        // Проверяем наличие сотрудника в БД
        String^ checkEmployeeQuery = String::Format("SELECT COUNT(*) FROM employees WHERE full_name = '{0}';", trimmedName);
        List<List<String^>^>^ employeeResults = gcnew List<List<String^>^>();

        if (GetSQLiteData(checkEmployeeQuery, employeeResults) && employeeResults->Count > 0) {
            List<String^>^ firstRow = employeeResults[0];
            if (firstRow->Count > 0) {
                int count = Convert::ToInt32(firstRow[0]);
                if (count > 0) {
                    throw gcnew System::ArgumentException("Сотрудник с таким именем уже существует");
                }
            }
        }

        // Проверяем наличие должности в БД
        String^ checkPositionQuery = String::Format("SELECT COUNT(*) FROM worktypes WHERE name = '{0}';", positionName);
        List<List<String^>^>^ positionResults = gcnew List<List<String^>^>();

        if (!GetSQLiteData(checkPositionQuery, positionResults) || positionResults->Count == 0) {
            throw gcnew System::ArgumentException("Указанная должность не существует");
        }

        List<String^>^ firstRow = positionResults[0];
        if (firstRow->Count == 0) {
            throw gcnew System::ArgumentException("Указанная должность не существует");
        }

        int positionCount = Convert::ToInt32(firstRow[0]);
        if (positionCount == 0) {
            throw gcnew System::ArgumentException("Указанная должность не существует");
        }

        if (workDays <= 0 || workDays > 30) {
            throw gcnew System::ArgumentException("Количество рабочих дней должно быть от 1 до 30");
        }

        // Добавляем в БД
        String^ insertQuery = String::Format(
            "INSERT INTO employees (full_name, position_name, work_days) VALUES ('{0}', '{1}', {2});",
            trimmedName, positionName, workDays);

        if (ExecuteSQLiteQuery(insertQuery)) {
            // Получаем информацию о должности для кэша
            String^ getSalaryQuery = String::Format("SELECT salary FROM worktypes WHERE name = '{0}';", positionName);
            List<List<String^>^>^ salaryResults = gcnew List<List<String^>^>();

            if (GetSQLiteData(getSalaryQuery, salaryResults) && salaryResults->Count > 0) {
                List<String^>^ salaryRow = salaryResults[0];
                if (salaryRow->Count > 0) {
                    int salary = Convert::ToInt32(salaryRow[0]);

                    // Создаем объекты и добавляем в кэш
                    WorkType^ wt = gcnew WorkType(positionName, salary);
                    if (!workTypes->ContainsKey(positionName)) {
                        workTypes->Add(positionName, wt);
                    }

                    Employee^ emp = gcnew Employee(trimmedName, wt, workDays);
                    employees->Add(trimmedName, emp);
                }
            }
            return true;
        }

        return false;
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

        // Проверяем наличие должности в БД
        String^ checkPositionQuery = String::Format("SELECT COUNT(*) FROM worktypes WHERE name = '{0}';", positionName);
        List<List<String^>^>^ positionResults = gcnew List<List<String^>^>();

        if (!GetSQLiteData(checkPositionQuery, positionResults) || positionResults->Count == 0) {
            throw gcnew System::ArgumentException("Указанная должность не существует");
        }

        // Получаем количество из результата
        List<String^>^ firstRow = positionResults[0];
        if (firstRow->Count == 0) {
            throw gcnew System::ArgumentException("Указанная должность не существует");
        }

        int positionCount = Convert::ToInt32(firstRow[0]);
        if (positionCount == 0) {
            throw gcnew System::ArgumentException("Указанная должность не существует");
        }

        if (workDays <= 0 || workDays > 30) {
            throw gcnew System::ArgumentException("Количество рабочих дней должно быть от 1 до 30");
        }

        // Проверяем существование старого сотрудника в БД
        String^ checkOldQuery = String::Format("SELECT COUNT(*) FROM employees WHERE full_name = '{0}';", oldName);
        List<List<String^>^>^ oldResults = gcnew List<List<String^>^>();

        if (!GetSQLiteData(checkOldQuery, oldResults) || oldResults->Count == 0) {
            return false;
        }

        List<String^>^ oldRow = oldResults[0];
        if (oldRow->Count == 0 || Convert::ToInt32(oldRow[0]) == 0) {
            return false;
        }

        // Проверяем, существует ли уже новый сотрудник (если имя изменилось)
        if (oldName != trimmedNewName) {
            String^ checkNewQuery = String::Format("SELECT COUNT(*) FROM employees WHERE full_name = '{0}';", trimmedNewName);
            List<List<String^>^>^ newResults = gcnew List<List<String^>^>();

            if (GetSQLiteData(checkNewQuery, newResults) && newResults->Count > 0) {
                List<String^>^ newRow = newResults[0];
                if (newRow->Count > 0) {
                    int count = Convert::ToInt32(newRow[0]);
                    if (count > 0) {
                        return false;
                    }
                }
            }
        }

        // Обновляем в БД
        String^ updateQuery = String::Format(
            "UPDATE employees SET full_name = '{0}', position_name = '{1}', work_days = {2} WHERE full_name = '{3}';",
            trimmedNewName, positionName, workDays, oldName);

        if (ExecuteSQLiteQuery(updateQuery)) {
            // Обновляем кэш
            if (employees->ContainsKey(oldName)) {
                Employee^ emp = employees[oldName];

                if (oldName != trimmedNewName) {
                    employees->Remove(oldName);
                    employees->Add(trimmedNewName, emp);
                    emp->FullName = trimmedNewName;
                }

                // Получаем информацию о должности
                String^ getSalaryQuery = String::Format("SELECT salary FROM worktypes WHERE name = '{0}';", positionName);
                List<List<String^>^>^ salaryResults = gcnew List<List<String^>^>();

                if (GetSQLiteData(getSalaryQuery, salaryResults) && salaryResults->Count > 0) {
                    List<String^>^ salaryRow = salaryResults[0];
                    if (salaryRow->Count > 0) {
                        int salary = Convert::ToInt32(salaryRow[0]);

                        // Обновляем или создаем объект должности
                        if (workTypes->ContainsKey(positionName)) {
                            emp->Position = workTypes[positionName];
                        }
                        else {
                            WorkType^ wt = gcnew WorkType(positionName, salary);
                            workTypes->Add(positionName, wt);
                            emp->Position = wt;
                        }
                    }
                }

                emp->WorkDays = workDays;
            }
            return true;
        }

        return false;
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка обновления сотрудника: " + ex->Message);
        throw;
    }
}

bool DataManager::DeleteEmployee(String^ name) {
    // Проверяем существование в БД
    String^ checkQuery = String::Format("SELECT COUNT(*) FROM employees WHERE full_name = '{0}';", name);
    List<List<String^>^>^ results = gcnew List<List<String^>^>();

    if (!GetSQLiteData(checkQuery, results) || results->Count == 0) {
        return false;
    }

    List<String^>^ firstRow = results[0];
    if (firstRow->Count == 0 || Convert::ToInt32(firstRow[0]) == 0) {
        return false;
    }

    // Удаляем из БД
    String^ deleteQuery = String::Format("DELETE FROM employees WHERE full_name = '{0}';", name);

    if (ExecuteSQLiteQuery(deleteQuery)) {
        // Обновляем кэш
        if (employees->ContainsKey(name)) {
            employees->Remove(name);
        }
        return true;
    }

    return false;
}

double DataManager::CalculateTotalSalary() {
    double total = 0;
    for each (KeyValuePair<String^, Employee^> emp in employees) {
        total += emp.Value->GetSalary();
    }
    return total;
}

bool DataManager::SaveToDatabase() {
    // В этой реализации все изменения сохраняются в БД немедленно
    // Этот метод оставлен для совместимости с интерфейсом
    return true;
}

bool DataManager::LoadFromDatabase() {
    try {
        LoadWorkTypesFromDB();
        LoadEmployeesFromDB();
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

bool DataManager::SaveToTextFile(String^ employeesFile, String^ workTypesFile) {
    try {
        System::Diagnostics::Debug::WriteLine("Сохранение в текстовые файлы:");
        System::Diagnostics::Debug::WriteLine("Файл сотрудников: " + employeesFile);
        System::Diagnostics::Debug::WriteLine("Файл должностей: " + workTypesFile);

        // Используем TextFileManager
        return TextFileManager::SaveAllData(employees, workTypes, employeesFile, workTypesFile);
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка сохранения в файл: " + ex->Message);
        return false;
    }
}

bool DataManager::LoadFromTextFile(String^ employeesFile, String^ workTypesFile) {
    try {
        System::Diagnostics::Debug::WriteLine("Загрузка из текстовых файлов:");
        System::Diagnostics::Debug::WriteLine("Файл сотрудников: " + employeesFile);
        System::Diagnostics::Debug::WriteLine("Файл должностей: " + workTypesFile);

        // Создаем временные словари
        Dictionary<String^, WorkType^>^ tempWorkTypes = gcnew Dictionary<String^, WorkType^>();
        Dictionary<String^, Employee^>^ tempEmployees = gcnew Dictionary<String^, Employee^>();

        // Загружаем данные
        bool success = TextFileManager::LoadAllData(tempEmployees, tempWorkTypes, employeesFile, workTypesFile);

        if (success) {
            int workTypesAdded = 0;
            int employeesAdded = 0;

            System::Diagnostics::Debug::WriteLine("Успешно загружено из файлов");
            System::Diagnostics::Debug::WriteLine(String::Format("Должностей: {0}", tempWorkTypes->Count));
            System::Diagnostics::Debug::WriteLine(String::Format("Сотрудников: {0}", tempEmployees->Count));

            // 1. Добавляем должности в БД и кэш (если их еще нет)
            for each (KeyValuePair<String^, WorkType^> pair in tempWorkTypes) {
                WorkType^ wt = pair.Value;

                // Проверяем, существует ли уже такая должность
                if (!workTypes->ContainsKey(wt->Name)) {
                    // Добавляем должность в БД
                    String^ insertWorkTypeQuery = String::Format(
                        "INSERT INTO worktypes (name, salary) VALUES ('{0}', {1});",
                        wt->Name->Replace("'", "''"),  // Экранируем апострофы для SQL
                        wt->Salary);

                    if (ExecuteSQLiteQuery(insertWorkTypeQuery)) {
                        // Добавляем в кэш
                        workTypes->Add(wt->Name, wt);
                        workTypesAdded++;
                    }
                    else {
                        System::Diagnostics::Debug::WriteLine(
                            String::Format("Ошибка при добавлении должности в БД: {0}", wt->Name));
                    }
                }
                else {
                    System::Diagnostics::Debug::WriteLine(
                        String::Format("Должность уже существует: {0}", wt->Name));
                }
            }

            // 2. Добавляем сотрудников в БД и кэш (если их еще нет)
            for each (KeyValuePair<String^, Employee^> pair in tempEmployees) {
                Employee^ emp = pair.Value;

                // Проверяем, существует ли уже такой сотрудник
                if (!employees->ContainsKey(emp->FullName)) {
                    // Проверяем, существует ли должность
                    if (workTypes->ContainsKey(emp->Position->Name)) {
                        // Добавляем сотрудника в БД
                        String^ insertEmployeeQuery = String::Format(
                            "INSERT INTO employees (full_name, position_name, work_days) VALUES ('{0}', '{1}', {2});",
                            emp->FullName->Replace("'", "''"),  // Экранируем апострофы для SQL
                            emp->Position->Name->Replace("'", "''"),
                            emp->WorkDays);

                        if (ExecuteSQLiteQuery(insertEmployeeQuery)) {
                            // Добавляем в кэш
                            employees->Add(emp->FullName, emp);
                            employeesAdded++;
                        }
                        else {
                            System::Diagnostics::Debug::WriteLine(
                                String::Format("Ошибка при добавлении сотрудника в БД: {0}", emp->FullName));
                        }
                    }
                    else {
                        System::Diagnostics::Debug::WriteLine(
                            String::Format("Пропускаем сотрудника {0}: должность {1} не найдена",
                                emp->FullName, emp->Position->Name));
                    }
                }
                else {
                    System::Diagnostics::Debug::WriteLine(
                        String::Format("Сотрудник уже существует: {0}", emp->FullName));
                }
            }

            System::Diagnostics::Debug::WriteLine(
                String::Format("Добавлено в БД: {0} должностей, {1} сотрудников",
                    workTypesAdded, employeesAdded));

            return true;
        }
        else {
            System::Diagnostics::Debug::WriteLine("Не удалось загрузить данные из файлов");
            return false;
        }
    }
    catch (Exception^ ex) {
        System::Diagnostics::Debug::WriteLine("Ошибка загрузки из файла: " + ex->Message);
        return false;
    }
}

bool DataManager::SaveToTextFile() {
    return SaveToTextFile("employees.txt", "worktypes.txt");
}

bool DataManager::LoadFromTextFile() {
    return LoadFromTextFile("employees.txt", "worktypes.txt");
}