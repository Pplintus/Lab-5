#include "Employee.h"

using namespace SalaryDepartment;

Employee::Employee() : fullName(""), position(nullptr), workDays(0) {}

Employee::Employee(String^ name, WorkType^ position, int days)
    : fullName(name), position(position), workDays(days) {
}

double Employee::GetSalary() {
    if (position == nullptr) return 0;
    return (workDays * position->Salary) / 30.0;
}