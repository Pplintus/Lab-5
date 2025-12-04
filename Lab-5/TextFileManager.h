#pragma once
#include "Employee.h"
#include "WorkType.h"

namespace SalaryDepartment {

    using namespace System;
    using namespace System::Collections::Generic;
    using namespace System::IO;
    using namespace System::Text;

    public ref class TextFileManager {
    private:
        static String^ defaultEmployeesFile = "employees.txt";
        static String^ defaultWorkTypesFile = "worktypes.txt";

    public:
        TextFileManager();

        // Методы для работы с файлами сотрудников
        static bool SaveEmployeesToFile(Dictionary<String^, Employee^>^ employees, String^ filePath);
        static bool LoadEmployeesFromFile(Dictionary<String^, Employee^>^% employees, Dictionary<String^, WorkType^>^ workTypes, String^ filePath);

        // Методы для работы с файлами должностей
        static bool SaveWorkTypesToFile(Dictionary<String^, WorkType^>^ workTypes, String^ filePath);
        static bool LoadWorkTypesFromFile(Dictionary<String^, WorkType^>^% workTypes, String^ filePath);

        // Методы для работы с обоими файлами одновременно
        static bool SaveAllData(Dictionary<String^, Employee^>^ employees,
            Dictionary<String^, WorkType^>^ workTypes,
            String^ employeesFile,
            String^ workTypesFile);

        static bool LoadAllData(Dictionary<String^, Employee^>^% employees,
            Dictionary<String^, WorkType^>^% workTypes,
            String^ employeesFile,
            String^ workTypesFile);

        // Методы с путями по умолчанию
        static bool SaveAllDataToDefault(Dictionary<String^, Employee^>^ employees,
            Dictionary<String^, WorkType^>^ workTypes);
        static bool LoadAllDataFromDefault(Dictionary<String^, Employee^>^% employees,
            Dictionary<String^, WorkType^>^% workTypes);

        // Геттеры для путей по умолчанию
        static property String^ DefaultEmployeesFile {
            String^ get() { return defaultEmployeesFile; }
        }

        static property String^ DefaultWorkTypesFile {
            String^ get() { return defaultWorkTypesFile; }
        }
    };
}