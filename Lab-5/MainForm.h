#pragma once
#include "DataManager.h"

namespace SalaryDepartmentApp {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace SalaryDepartment;

    public ref class MainForm : public Form {
    private:
        DataManager^ dataManager;
        DataGridView^ dataGridEmployees;
        DataGridView^ dataGridWorkTypes;
        TabControl^ tabControl;
        Label^ lblTotalSalary;

        void InitializeComponent();
        void RefreshData();

        // Обработчики событий
        void MenuLoad_Click(Object^ sender, EventArgs^ e);
        void MenuSave_Click(Object^ sender, EventArgs^ e);
        void MenuExit_Click(Object^ sender, EventArgs^ e);
        void AddEmployee_Click(Object^ sender, EventArgs^ e);
        void AddWorkType_Click(Object^ sender, EventArgs^ e);
        void DeleteEmployee_Click(Object^ sender, EventArgs^ e);
        void DeleteWorkType_Click(Object^ sender, EventArgs^ e);
        void SortByName_Click(Object^ sender, EventArgs^ e);
        void SortBySalary_Click(Object^ sender, EventArgs^ e);
        void ShowTotal_Click(Object^ sender, EventArgs^ e);

    public:
        MainForm();
    };
}