#include "EditForm.h"
#include "DataManager.h"

using namespace SalaryDepartmentApp;
using namespace SalaryDepartment;

EditForm::EditForm(FormMode mode) : mode(mode) {
    isOK = false;
    InitializeComponent();
    SetupForm();
}

EditForm::EditForm(FormMode mode, String^ editName) : mode(mode), originalName(editName) {
    isOK = false;
    InitializeComponent();
    SetupForm();

    // Загружаем данные для редактирования
    DataManager^ dm = gcnew DataManager();
    dm->LoadFromFiles("employees.txt", "worktypes.txt");

    if (mode == FormMode::EditEmployee && dm->Employees->ContainsKey(editName)) {
        Employee^ emp = dm->Employees[editName];
        txtName->Text = emp->FullName;
        numDays->Value = emp->WorkDays;

        // Выбираем должность в комбобоксе
        for (int i = 0; i < cmbPosition->Items->Count; i++) {
            if (cmbPosition->Items[i]->ToString() == emp->Position->Name) {
                cmbPosition->SelectedIndex = i;
                break;
            }
        }
    }
    else if (mode == FormMode::EditWorkType && dm->WorkTypes->ContainsKey(editName)) {
        WorkType^ wt = dm->WorkTypes[editName];
        txtName->Text = wt->Name;
        numSalary->Value = wt->Salary;
    }
}

void EditForm::InitializeComponent() {
    this->Text = "Форма редактирования";
    this->Size = System::Drawing::Size(400, 300);
    this->StartPosition = FormStartPosition::CenterParent;
    this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog; // Исправлено!
    this->MaximizeBox = false;
    this->MinimizeBox = false;

    lblTitle = gcnew Label();
    lblTitle->Font = gcnew System::Drawing::Font("Arial", 14, FontStyle::Bold);
    lblTitle->Location = System::Drawing::Point(20, 20);
    lblTitle->Size = System::Drawing::Size(350, 25);

    lblField1 = gcnew Label();
    lblField1->Location = System::Drawing::Point(20, 60);
    lblField1->Size = System::Drawing::Size(150, 20);

    txtName = gcnew TextBox();
    txtName->Location = System::Drawing::Point(20, 80);
    txtName->Size = System::Drawing::Size(350, 20);

    lblField2 = gcnew Label();
    lblField2->Location = System::Drawing::Point(20, 110);
    lblField2->Size = System::Drawing::Size(150, 20);

    cmbPosition = gcnew ComboBox();
    cmbPosition->Location = System::Drawing::Point(20, 130);
    cmbPosition->Size = System::Drawing::Size(350, 21);
    cmbPosition->DropDownStyle = ComboBoxStyle::DropDownList;

    numSalary = gcnew NumericUpDown();
    numSalary->Location = System::Drawing::Point(20, 130);
    numSalary->Size = System::Drawing::Size(350, 20);
    numSalary->Minimum = 10000;
    numSalary->Maximum = 1000000;
    numSalary->Value = 30000;
    numSalary->Visible = false;

    lblField3 = gcnew Label();
    lblField3->Location = System::Drawing::Point(20, 160);
    lblField3->Size = System::Drawing::Size(150, 20);

    numDays = gcnew NumericUpDown();
    numDays->Location = System::Drawing::Point(20, 180);
    numDays->Size = System::Drawing::Size(350, 20);
    numDays->Minimum = 1;
    numDays->Maximum = 30;
    numDays->Value = 20;
    numDays->Visible = false;

    btnOK = gcnew Button();
    btnOK->Text = "OK";
    btnOK->Location = System::Drawing::Point(200, 220);
    btnOK->Size = System::Drawing::Size(80, 30);
    btnOK->Click += gcnew System::EventHandler(this, &EditForm::OK_Click);

    btnCancel = gcnew Button();
    btnCancel->Text = "Отмена";
    btnCancel->Location = System::Drawing::Point(290, 220);
    btnCancel->Size = System::Drawing::Size(80, 30);
    btnCancel->Click += gcnew System::EventHandler(this, &EditForm::Cancel_Click);

    this->Controls->Add(lblTitle);
    this->Controls->Add(lblField1);
    this->Controls->Add(txtName);
    this->Controls->Add(lblField2);
    this->Controls->Add(cmbPosition);
    this->Controls->Add(numSalary);
    this->Controls->Add(lblField3);
    this->Controls->Add(numDays);
    this->Controls->Add(btnOK);
    this->Controls->Add(btnCancel);
}

void EditForm::SetupForm() {
    DataManager^ dm = gcnew DataManager();
    dm->LoadFromFiles("employees.txt", "worktypes.txt");

    switch (mode) {
    case FormMode::AddEmployee:
        lblTitle->Text = "Добавить сотрудника";
        lblField1->Text = "ФИО:";
        lblField2->Text = "Должность:";
        lblField3->Text = "Отработано дней:";

        cmbPosition->Visible = true;
        numSalary->Visible = false;
        numDays->Visible = true;

        // Заполняем комбобокс должностями
        cmbPosition->Items->Clear();
        for each (KeyValuePair<String^, WorkType^> wt in dm->WorkTypes) {
            cmbPosition->Items->Add(wt.Key);
        }
        if (cmbPosition->Items->Count > 0)
            cmbPosition->SelectedIndex = 0;
        break;

    case FormMode::EditEmployee:
        lblTitle->Text = "Редактировать сотрудника";
        lblField1->Text = "ФИО:";
        lblField2->Text = "Должность:";
        lblField3->Text = "Отработано дней:";

        cmbPosition->Visible = true;
        numSalary->Visible = false;
        numDays->Visible = true;

        cmbPosition->Items->Clear();
        for each (KeyValuePair<String^, WorkType^> wt in dm->WorkTypes) {
            cmbPosition->Items->Add(wt.Key);
        }
        break;

    case FormMode::AddWorkType:
        lblTitle->Text = "Добавить должность";
        lblField1->Text = "Название:";
        lblField2->Text = "Оклад:";
        lblField3->Text = "";
        lblField3->Visible = false;

        cmbPosition->Visible = false;
        numSalary->Visible = true;
        numDays->Visible = false;
        break;

    case FormMode::EditWorkType:
        lblTitle->Text = "Редактировать должность";
        lblField1->Text = "Название:";
        lblField2->Text = "Оклад:";
        lblField3->Text = "";
        lblField3->Visible = false;

        cmbPosition->Visible = false;
        numSalary->Visible = true;
        numDays->Visible = false;
        break;
    }
}

bool EditForm::ValidateInput() {
    if (System::String::IsNullOrWhiteSpace(txtName->Text)) {
        System::Windows::Forms::MessageBox::Show("Поле не может быть пустым!", "Ошибка",
            MessageBoxButtons::OK, MessageBoxIcon::Error);
        return false;
    }

    if (mode == FormMode::AddEmployee || mode == FormMode::EditEmployee) {
        if (cmbPosition->SelectedItem == nullptr) {
            System::Windows::Forms::MessageBox::Show("Выберите должность!", "Ошибка",
                MessageBoxButtons::OK, MessageBoxIcon::Error);
            return false;
        }
    }

    return true;
}

void EditForm::OK_Click(System::Object^ sender, System::EventArgs^ e) {
    if (ValidateInput()) {
        isOK = true;
        this->DialogResult = System::Windows::Forms::DialogResult::OK;
        this->Close();
    }
}

void EditForm::Cancel_Click(System::Object^ sender, System::EventArgs^ e) {
    isOK = false;
    this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
    this->Close();
}