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
    if (String::IsNullOrWhiteSpace(name) || salary <= 0)
        return false;

    if (workTypes->ContainsKey(name))
        return false;

    workTypes->Add(name, gcnew WorkType(name, salary));
    return true;
}

bool DataManager::UpdateWorkType(String^ oldName, String^ newName, int salary) {
    if (String::IsNullOrWhiteSpace(newName) || salary <= 0)
        return false;

    if (!workTypes->ContainsKey(oldName))
        return false;

    // Проверяем, используется ли должность
    for each (KeyValuePair<String^, Employee^> emp in employees) {
        if (emp.Value->Position->Name == oldName)
            return false;
    }

    workTypes->Remove(oldName);
    workTypes->Add(newName, gcnew WorkType(newName, salary));
    return true;
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
    if (String::IsNullOrWhiteSpace(name) || workDays <= 0 || workDays > 30)
        return false;

    if (!workTypes->ContainsKey(positionName) || employees->ContainsKey(name))
        return false;

    Employee^ emp = gcnew Employee(name, workTypes[positionName], workDays);
    employees->Add(name, emp);
    return true;
}

bool DataManager::UpdateEmployee(String^ oldName, String^ newName, String^ positionName, int workDays) {
    if (String::IsNullOrWhiteSpace(newName) || workDays <= 0 || workDays > 30)
        return false;

    if (!workTypes->ContainsKey(positionName) || !employees->ContainsKey(oldName))
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