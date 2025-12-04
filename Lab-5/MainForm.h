#pragma once
#include "DataManager.h"
#include "EditForm.h"
#include "TextFileManager.h"

namespace SalaryDepartmentApp {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace SalaryDepartment;

    public enum class Theme {
        Light,
        Dark
    };

    public enum class ViewMode {
        Employees,
        WorkTypes
    };

    public ref class MainForm : public Form {
    private:
        DataManager^ dataManager;
        DataGridView^ dataGrid;
        Label^ lblTotalSalary;
        Theme currentTheme;
        ViewMode currentViewMode;

        System::Void ApplyTheme(Theme theme);
        System::Void SwitchView(ViewMode mode);
        System::Void RefreshData();
        System::Void SetupDataGrid();

        // Обработчики событий
        System::Void MenuLoad_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void MenuSave_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void MenuExit_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void AddEmployee_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void AddWorkType_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void DeleteEmployee_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void DeleteWorkType_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void SortByName_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void SortBySalary_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void ShowTotal_Click(System::Object^ sender, System::EventArgs^ e);

        // Обработчик для DataGridView
        System::Void DataGrid_CellDoubleClick(System::Object^ sender, DataGridViewCellEventArgs^ e);

        // Методы для редактирования
        System::Void EditSelectedEmployee();
        System::Void EditSelectedWorkType();

        // Универсальные обработчики
        System::Void Add_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void Edit_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void Delete_Click(System::Object^ sender, System::EventArgs^ e);

        System::Void MenuLightTheme_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void MenuDarkTheme_Click(System::Object^ sender, System::EventArgs^ e);

        // Обработчики переключения вида
        System::Void ShowEmployees_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void ShowWorkTypes_Click(System::Object^ sender, System::EventArgs^ e);

    public:
        MainForm();
    };
}