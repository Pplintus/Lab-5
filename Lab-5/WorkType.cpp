#include "WorkType.h"

using namespace SalaryDepartment;

WorkType::WorkType() : name(""), salary(0) {}

WorkType::WorkType(String^ name, int salary) : name(name), salary(salary) {}