#pragma once

namespace SalaryDepartmentApp {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;

    public enum class FormMode {
        AddEmployee,
        EditEmployee,
        AddWorkType,
        EditWorkType
    };

    public ref class EditForm : public Form {
    private:
        FormMode mode;
        String^ originalName;
        bool isOK;

        // Ёлементы формы
        TextBox^ txtName;
        ComboBox^ cmbPosition;
        NumericUpDown^ numDays;
        NumericUpDown^ numSalary;
        Label^ lblTitle;
        Label^ lblField1;
        Label^ lblField2;
        Label^ lblField3;
        Button^ btnOK;
        Button^ btnCancel;

        void InitializeComponent();
        void SetupForm();

        void OK_Click(Object^ sender, EventArgs^ e);
        void Cancel_Click(Object^ sender, EventArgs^ e);

        bool ValidateInput();

    public:
        property String^ EmployeeName {
            String^ get() { return txtName->Text; }
        }

        property String^ PositionName {
            String^ get() {
                if (cmbPosition->SelectedItem != nullptr)
                    return cmbPosition->SelectedItem->ToString();
                return "";
            }
        }

        property int WorkDays {
            int get() { return (int)numDays->Value; }
        }

        property int Salary {
            int get() { return (int)numSalary->Value; }
        }

        property bool IsOK {
            bool get() { return isOK; }
        }

        // ѕерегруженные конструкторы
        EditForm(FormMode mode);
        EditForm(FormMode mode, String^ editName);
    };
}