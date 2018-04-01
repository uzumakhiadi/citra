#pragma once

#include <functional>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>

namespace Ui {
class CheatSearch;
}

class FoundItem {
public:
    std::string address;
    std::string value;
};

class CheatSearch : public QDialog {
    Q_OBJECT

public:
    explicit CheatSearch(QWidget* parent = 0);
    ~CheatSearch();

private:
    Ui::CheatSearch* ui;
    std::vector<FoundItem> previous_found;
    void OnScan(bool isNextScan);
    void OnScanTypeChanged(int index);
    void OnValueTypeChanged(int index);
    void OnHexCheckedChanged(bool checked);
    void LoadTable(std::vector<FoundItem> items);

    template <typename T>
    std::vector<FoundItem> FirstSearch(const T value, std::function<bool(int, int, int)> comparer);

    template <typename T>
    std::vector<FoundItem> NextSearch(
        const T value, std::function<bool(int, int, int)> comparer,
        const std::vector<FoundItem> previous_found);

    bool Equals(int a, int b, int c);
    bool LessThan(int a, int b, int c);
    bool GreaterThan(int a, int b, int c);
    bool Between(int min, int max, int value);
};

class ModifyAddressDialog : public QDialog {
    Q_OBJECT

public:
    ModifyAddressDialog(QWidget* parent, std::string address, int type, std::string value);
    ~ModifyAddressDialog();

    QString return_value;

private:
    QLineEdit* address_block;
    QComboBox* type_select;
    QLineEdit* value_block;

    void OnOkClicked();
};
