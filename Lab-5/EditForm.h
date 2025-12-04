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

        // Элементы формы
        TextBox^ txtName;
        ComboBox^ cmbPosition;
        NumericUpDown^ numDays;
        TextBox^ txtSalary;      // ИЗМЕНЕНО: TextBox вместо NumericUpDown
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
        void ValidateInput(Object^ sender, EventArgs^ e);
        bool ValidateInput();
        void ShowError(String^ message, Control^ control);
        void txtSalary_KeyPress(Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e);  // ДОБАВЛЕНО

    public:
        property String^ EmployeeName {
            String^ get() {
                if (txtName != nullptr)
                    return txtName->Text->Trim();
                return "";
            }
        }

        property String^ PositionName {
            String^ get() {
                if (cmbPosition != nullptr && cmbPosition->SelectedItem != nullptr)
                    return cmbPosition->SelectedItem->ToString();
                return "";
            }
        }

        property int WorkDays {
            int get() {
                if (numDays != nullptr)
                    return Decimal::ToInt32(numDays->Value);
                return 0;
            }
        }

        property int Salary {
            int get() {
                if (txtSalary != nullptr && !String::IsNullOrWhiteSpace(txtSalary->Text)) {
                    int result = 0;
                    if (Int32::TryParse(txtSalary->Text, result))
                        return result;
                }
                return 0;
            }
        }

        property bool IsOK {
            bool get() { return isOK; }
        }

        // Перегруженные конструкторы
        EditForm(FormMode mode);
        EditForm(FormMode mode, String^ editName);
    };
}