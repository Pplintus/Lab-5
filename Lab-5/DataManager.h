#pragma once
#define WIN32_LEAN_AND_MEAN
#include "Employee.h"
#include "WorkType.h"
#include "TextFileManager.h"

using namespace System;
using namespace System::Collections::Generic;

namespace SalaryDepartment {

    public ref class DataManager {
    private:
        Dictionary<String^, Employee^>^ employees;
        Dictionary<String^, WorkType^>^ workTypes;

        // Приватные методы для работы с БД
        bool InitializeDatabase();
        void LoadWorkTypesFromDB();
        void LoadEmployeesFromDB();

        // Вспомогательные статические методы для работы с SQLite
        static bool ExecuteSQLiteQuery(String^ query);
        static bool GetSQLiteData(String^ query, List<List<String^>^>^% results);

    public:
        DataManager();
        DataManager(String^ databasePath);

        property Dictionary<String^, Employee^>^ Employees {
            Dictionary<String^, Employee^>^ get() { return employees; }
        }

        property Dictionary<String^, WorkType^>^ WorkTypes {
            Dictionary<String^, WorkType^>^ get() { return workTypes; }
        }

        // Методы для работы с должностями
        bool AddWorkType(String^ name, int salary);
        bool UpdateWorkType(String^ oldName, String^ newName, int salary);
        bool DeleteWorkType(String^ name);

        // Методы для работы с сотрудниками
        bool AddEmployee(String^ name, String^ positionName, int workDays);
        bool UpdateEmployee(String^ oldName, String^ newName, String^ positionName, int workDays);
        bool DeleteEmployee(String^ name);

        // Расчеты
        double CalculateTotalSalary();

        // Работа с БД
        bool SaveToDatabase();
        bool LoadFromDatabase();

        // Работа с текстовыми файлами
        bool SaveToTextFile(String^ employeesFile, String^ workTypesFile);
        bool LoadFromTextFile(String^ employeesFile, String^ workTypesFile);
        bool SaveToTextFile(); // Использует пути по умолчанию
        bool LoadFromTextFile(); // Использует пути по умолчанию

        // Сортировка
        List<Employee^>^ GetEmployeesSortedByName();
        List<Employee^>^ GetEmployeesSortedBySalary();
        List<WorkType^>^ GetWorkTypesSortedByName();
    };
}