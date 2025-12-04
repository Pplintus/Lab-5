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
    dm->LoadFromDatabase();

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
        txtSalary->Text = wt->Salary.ToString();  // Заполняем TextBox
    }
}

void EditForm::InitializeComponent() {
    this->Text = "Форма редактирования";
    this->Size = System::Drawing::Size(400, 300);
    this->StartPosition = FormStartPosition::CenterParent;
    this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
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
    txtName->MaxLength = 100;
    txtName->TextChanged += gcnew System::EventHandler(this, &EditForm::ValidateInput);

    lblField2 = gcnew Label();
    lblField2->Location = System::Drawing::Point(20, 110);
    lblField2->Size = System::Drawing::Size(150, 20);

    cmbPosition = gcnew ComboBox();
    cmbPosition->Location = System::Drawing::Point(20, 130);
    cmbPosition->Size = System::Drawing::Size(350, 21);
    cmbPosition->DropDownStyle = ComboBoxStyle::DropDownList;
    cmbPosition->SelectedIndexChanged += gcnew System::EventHandler(this, &EditForm::ValidateInput);

    // ИСПРАВЛЕНО: TextBox для оклада вместо NumericUpDown
    txtSalary = gcnew TextBox();
    txtSalary->Location = System::Drawing::Point(20, 130);
    txtSalary->Size = System::Drawing::Size(350, 20);
    txtSalary->Visible = false;
    txtSalary->TextChanged += gcnew System::EventHandler(this, &EditForm::ValidateInput);
    txtSalary->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &EditForm::txtSalary_KeyPress);

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
    numDays->ValueChanged += gcnew System::EventHandler(this, &EditForm::ValidateInput);

    btnOK = gcnew Button();
    btnOK->Text = "OK";
    btnOK->Location = System::Drawing::Point(200, 220);
    btnOK->Size = System::Drawing::Size(80, 30);
    btnOK->Click += gcnew System::EventHandler(this, &EditForm::OK_Click);
    btnOK->Enabled = false;

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
    this->Controls->Add(txtSalary);
    this->Controls->Add(lblField3);
    this->Controls->Add(numDays);
    this->Controls->Add(btnOK);
    this->Controls->Add(btnCancel);
}

void EditForm::txtSalary_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e) {
    // Разрешаем только цифры, Backspace (8) и Delete (127)
    if (!Char::IsDigit(e->KeyChar) && e->KeyChar != 8 && e->KeyChar != 127) {
        e->Handled = true;
    }

    // Ограничение на длину (максимум 7 цифр для оклада до 9,999,999)
    TextBox^ tb = dynamic_cast<TextBox^>(sender);
    if (tb != nullptr && tb->Text->Length >= 7 && e->KeyChar != 8 && e->KeyChar != 127) {
        e->Handled = true;
    }
}

void EditForm::SetupForm() {
    DataManager^ dm = gcnew DataManager();
    dm->LoadFromDatabase();

    switch (mode) {
    case FormMode::AddEmployee:
        lblTitle->Text = "Добавить сотрудника";
        lblField1->Text = "ФИО:";
        lblField2->Text = "Должность:";
        lblField3->Text = "Отработано дней:";

        cmbPosition->Visible = true;
        txtSalary->Visible = false;
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
        txtSalary->Visible = false;
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
        txtSalary->Visible = true;
        txtSalary->Text = "30000"; // Значение по умолчанию
        numDays->Visible = false;
        break;

    case FormMode::EditWorkType:
        lblTitle->Text = "Редактировать должность";
        lblField1->Text = "Название:";
        lblField2->Text = "Оклад:";
        lblField3->Text = "";
        lblField3->Visible = false;

        cmbPosition->Visible = false;
        txtSalary->Visible = true;
        numDays->Visible = false;
        break;
    }

    // Вызываем валидацию для начального состояния
    ValidateInput(nullptr, nullptr);
}

void EditForm::ValidateInput(Object^ sender, EventArgs^ e) {
    try {
        bool isValid = true;

        // Очищаем подсветку
        txtName->BackColor = System::Drawing::Color::White;
        if (cmbPosition != nullptr) cmbPosition->BackColor = System::Drawing::Color::White;
        if (numDays != nullptr) numDays->BackColor = System::Drawing::Color::White;
        if (txtSalary != nullptr) txtSalary->BackColor = System::Drawing::Color::White;

        // Проверка имени
        String^ name = txtName->Text->Trim();
        if (System::String::IsNullOrWhiteSpace(name)) {
            isValid = false;
            txtName->BackColor = System::Drawing::Color::LightPink;
        }
        else if (name->Length < 2) {
            isValid = false;
            txtName->BackColor = System::Drawing::Color::LightPink;
        }
        else if (name->Length > 100) {
            isValid = false;
            txtName->BackColor = System::Drawing::Color::LightPink;
        }
        else {
            // Проверка на специальные символы
            array<Char>^ invalidChars = { '/', '\\', ':', '*', '?', '"', '<', '>', '|', '@', '#', '$', '%', '^', '&', '=', '+' };
            for each (Char c in invalidChars) {
                if (name->Contains(c.ToString())) {
                    isValid = false;
                    txtName->BackColor = System::Drawing::Color::LightPink;
                    break;
                }
            }
        }

        // Проверка для сотрудников
        if (mode == FormMode::AddEmployee || mode == FormMode::EditEmployee) {
            if (cmbPosition->SelectedItem == nullptr) {
                isValid = false;
                cmbPosition->BackColor = System::Drawing::Color::LightPink;
            }

            if (numDays->Value < 1 || numDays->Value > 30) {
                isValid = false;
                numDays->BackColor = System::Drawing::Color::LightPink;
            }
        }

        // Проверка для должностей (оклад в TextBox)
        if (mode == FormMode::AddWorkType || mode == FormMode::EditWorkType) {
            if (txtSalary != nullptr) {
                int salaryValue = 0;
                if (Int32::TryParse(txtSalary->Text, salaryValue)) {
                    if (salaryValue >= 10000 && salaryValue <= 1000000) {
                        // Все хорошо
                    }
                    else {
                        isValid = false;
                        txtSalary->BackColor = System::Drawing::Color::LightPink;
                    }
                }
                else {
                    isValid = false;
                    txtSalary->BackColor = System::Drawing::Color::LightPink;
                }
            }
        }

        // Включаем/выключаем кнопку OK
        btnOK->Enabled = isValid;

    }
    catch (Exception^ ex) {
        btnOK->Enabled = false;
        System::Diagnostics::Debug::WriteLine("Ошибка валидации: " + ex->Message);
    }
}

bool EditForm::ValidateInput() {
    try {
        // Очищаем все подсветки ошибок
        txtName->BackColor = System::Drawing::Color::White;
        if (cmbPosition != nullptr) cmbPosition->BackColor = System::Drawing::Color::White;
        if (numDays != nullptr) numDays->BackColor = System::Drawing::Color::White;
        if (txtSalary != nullptr) txtSalary->BackColor = System::Drawing::Color::White;

        // Проверка 1: Пустое имя
        String^ name = txtName->Text->Trim();
        if (System::String::IsNullOrWhiteSpace(name)) {
            ShowError("Поле 'Имя' не может быть пустым!", txtName);
            return false;
        }

        // Проверка 2: Длина имени
        if (name->Length < 2) {
            ShowError("Имя должно содержать минимум 2 символа!", txtName);
            return false;
        }

        if (name->Length > 100) {
            ShowError("Имя не может быть длиннее 100 символов!", txtName);
            return false;
        }

        // Проверка 3: Запрещенные символы
        array<Char>^ invalidChars = { '/', '\\', ':', '*', '?', '"', '<', '>', '|', '@', '#', '$', '%', '^', '&', '=', '+' };
        for each (Char c in invalidChars) {
            if (name->Contains(c.ToString())) {
                ShowError("Имя не может содержать специальные символы: / \\ : * ? \" < > | @ # $ % ^ & = +", txtName);
                return false;
            }
        }

        // Проверка 4: Только пробелы
        if (name->Replace(" ", "")->Length == 0) {
            ShowError("Имя не может состоять только из пробелов!", txtName);
            return false;
        }

        // Проверка 5: Двойные пробелы
        if (name->Contains("  ")) {
            ShowError("Имя не может содержать двойные пробелы!", txtName);
            return false;
        }

        // Проверки для сотрудников
        if (mode == FormMode::AddEmployee || mode == FormMode::EditEmployee) {
            if (cmbPosition->SelectedItem == nullptr) {
                ShowError("Выберите должность!", cmbPosition);
                return false;
            }

            if (numDays->Value < 1 || numDays->Value > 30) {
                ShowError("Количество дней должно быть от 1 до 30!", numDays);
                return false;
            }
        }

        // Проверки для должностей
        if (mode == FormMode::AddWorkType || mode == FormMode::EditWorkType) {
            if (txtSalary != nullptr) {
                int salaryValue = 0;
                if (Int32::TryParse(txtSalary->Text, salaryValue)) {
                    if (salaryValue < 10000 || salaryValue > 1000000) {
                        ShowError("Оклад должен быть от 10,000 до 1,000,000 руб.!", txtSalary);
                        return false;
                    }
                }
                else {
                    ShowError("Введите корректную сумму оклада (только цифры)!", txtSalary);
                    return false;
                }
            }
        }

        return true;

    }
    catch (Exception^ ex) {
        System::Windows::Forms::MessageBox::Show(
            "Ошибка валидации: " + ex->Message,
            "Ошибка",
            System::Windows::Forms::MessageBoxButtons::OK,
            System::Windows::Forms::MessageBoxIcon::Error
        );
        return false;
    }
}

void EditForm::ShowError(String^ message, Control^ control) {
    System::Windows::Forms::MessageBox::Show(
        message,
        "Ошибка ввода",
        System::Windows::Forms::MessageBoxButtons::OK,
        System::Windows::Forms::MessageBoxIcon::Warning
    );

    if (control != nullptr) {
        control->BackColor = System::Drawing::Color::LightPink;
        control->Focus();
    }
}

void EditForm::OK_Click(System::Object^ sender, System::EventArgs^ e) {
    try {
        if (ValidateInput()) {
            isOK = true;
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->Close();
        }
    }
    catch (Exception^ ex) {
        System::Windows::Forms::MessageBox::Show(
            "Ошибка при сохранении: " + ex->Message,
            "Ошибка",
            System::Windows::Forms::MessageBoxButtons::OK,
            System::Windows::Forms::MessageBoxIcon::Error
        );
    }
}

void EditForm::Cancel_Click(System::Object^ sender, System::EventArgs^ e) {
    isOK = false;
    this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
    this->Close();
}