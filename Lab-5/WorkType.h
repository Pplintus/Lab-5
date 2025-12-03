#pragma once

using namespace System;

namespace SalaryDepartment {

    public ref class WorkType {
    private:
        String^ name;
        int salary;

    public:
        WorkType();
        WorkType(String^ name, int salary);

        property String^ Name {
            String^ get() { return name; }
            void set(String^ value) { name = value; }
        }

        property int Salary {
            int get() { return salary; }
            void set(int value) { salary = value; }
        }
    };
}