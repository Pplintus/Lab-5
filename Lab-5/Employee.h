#pragma once
#include "WorkType.h"

using namespace System;

namespace SalaryDepartment {

    public ref class Employee {
    private:
        String^ fullName;
        WorkType^ position;
        int workDays;

    public:
        Employee();
        Employee(String^ name, WorkType^ position, int days);

        property String^ FullName {
            String^ get() { return fullName; }
            void set(String^ value) { fullName = value; }
        }

        property WorkType^ Position {
            WorkType^ get() { return position; }
            void set(WorkType^ value) { position = value; }
        }

        property int WorkDays {
            int get() { return workDays; }
            void set(int value) { workDays = value; }
        }

        double GetSalary();
    };
}