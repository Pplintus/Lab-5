#pragma once
#include "Employee.h"
#include "WorkType.h"

using namespace System;
using namespace System::Collections::Generic;

namespace SalaryDepartment {

    public ref class DataManager {
    private:
        Dictionary<String^, Employee^>^ employees;
        Dictionary<String^, WorkType^>^ workTypes;

    public:
        DataManager();

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

        // Работа с файлами
        bool SaveToFiles(String^ employeesFile, String^ workTypesFile);
        bool LoadFromFiles(String^ employeesFile, String^ workTypesFile);

        // Сортировка
        List<Employee^>^ GetEmployeesSortedByName();
        List<Employee^>^ GetEmployeesSortedBySalary();
        List<WorkType^>^ GetWorkTypesSortedByName();
    };
}