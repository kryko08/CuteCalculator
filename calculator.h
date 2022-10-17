#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Calculator; }
QT_END_NAMESPACE

class Calculator : public QMainWindow
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);
    ~Calculator();

private:
    Ui::Calculator *ui;
    void showError();
    bool performOperation(double operand, const QList<QString> &pendingOperator);

/* Functions to be called when signal is emitted (e.g. Button is clicked)*/
private slots:
    void equalsClicked();
    void pointClicked();
    void changesignClicked();

    void backspaceClicked();
    void clearClicked();
    void clearallClicked();

    void multiplicativeClicked();
    void additiveClicked();
    void digitClicked();

    void openingparenthesisClicked();
    void closingparenthesisClicked();
};
#endif // CALCULATOR_H
