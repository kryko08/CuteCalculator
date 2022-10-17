#include "calculator.h"
#include "./ui_calculator.h"


QVector<double> runningSum(10, 0.0);
QVector<double> runningFactor(10, 0.0);

// This calculator supports 10 nested parenthesis at most
QVector<QString> runningAdditiveOper(10);
QVector<QString> runningMultiplicativeOper(10);
QVector<bool> awaitingOper(10, true);

int parenthesisLevel = 0;


Calculator::Calculator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);

    ui->Display->setText(QString::number(runningSum[parenthesisLevel]));

    // Connect Digit Buttons
    QPushButton * digitButtons[10];
    for(int i = 0; i < 10; ++i){
        QString buttonName = "Button" + QString::number(i);
        //Find Button in UI
        digitButtons[i] = Calculator::findChild<QPushButton*>(buttonName);
        connect(digitButtons[i], SIGNAL(released()), this, SLOT(digitClicked()));
    }

    // Connect the rest of buttons
    // additive operators
    connect(ui->Add, SIGNAL(released()), this, SLOT(additiveClicked()));
    connect(ui->Subtract, SIGNAL(released()), this, SLOT(additiveClicked()));

    // multiplicative operators
    connect(ui->Multiply, SIGNAL(released()), this, SLOT(multiplicativeClicked()));
    connect(ui->Divide, SIGNAL(released()), this, SLOT(multiplicativeClicked()));

    // clearing buttons
    connect(ui->Backspace, SIGNAL(released()), this, SLOT(backspaceClicked()));
    connect(ui->AllClear, SIGNAL(released()), this, SLOT(clearallClicked()));
    connect(ui->ClearEntry, SIGNAL(released()), this, SLOT(clearClicked()));

    // parenthesis
    connect(ui->LeftParenthesis, SIGNAL(released()), this, SLOT(openingparenthesisClicked()));
    connect(ui->RightParenthesis, SIGNAL(released()), this, SLOT(closingparenthesisClicked()));

    // Other
    connect(ui->ChangeSign, SIGNAL(released()), this, SLOT(changesignClicked()));
    connect(ui->Point, SIGNAL(released()), this, SLOT(pointClicked()));
    connect(ui->Equals, SIGNAL(released()), this, SLOT(equalsClicked()));
}


Calculator::~Calculator()
{
    delete ui;
}


void Calculator::digitClicked(){
    QPushButton *button = (QPushButton*)sender();
    /* Get button value*/
    int buttonValue = button->text().toInt();
    /* Do nothing if zero button clicked and value on display is 0*/
    if(buttonValue == 0.0 && ui->Display->text() == "0"){
        return;
    }

    if (awaitingOper[parenthesisLevel]){
        ui->Display->clear();
        awaitingOper[parenthesisLevel] = false;
    }
    double displayNumber = (ui->Display->text() + button->text()).toDouble();
    // QString newdisplayValue = QString::number(displayNumber, 'g', 15);
    QString newdisplayValue = QString::number(displayNumber);
    ui->Display->setText(newdisplayValue);
}


void Calculator::additiveClicked(){
    QPushButton *button = (QPushButton*)sender();
    if(!button){
        return;
    }
    QString buttonValue = button->text();
    double operand = ui->Display->text().toDouble(); // current value on display

    // Multiplicative operator has higher priority, check for these operators first
    if(!runningMultiplicativeOper[parenthesisLevel].isEmpty()){
        if(!performOperation(operand, runningMultiplicativeOper)){
            showError();
            return;
        }
        ui->Display->setText(QString::number(runningFactor[parenthesisLevel]));
        operand = runningFactor[parenthesisLevel];
        runningFactor[parenthesisLevel] = 0.0;
        runningMultiplicativeOper[parenthesisLevel].clear();
    }

    if (!runningAdditiveOper[parenthesisLevel].isEmpty()) {
        if (!performOperation(operand, runningAdditiveOper)) {
            showError();
            return;
        }
            ui->Display->setText(QString::number(runningSum[parenthesisLevel]));
    } else {
            runningSum[parenthesisLevel] = operand;
    }
    runningAdditiveOper[parenthesisLevel] = buttonValue;
    awaitingOper[parenthesisLevel] = true;
}


void Calculator::multiplicativeClicked(){
    QPushButton *button = (QPushButton*)sender();
    if(!button){
        return;
    }
    QString buttonValue = button->text();
    double operand = ui->Display->text().toDouble();

    if(!runningMultiplicativeOper[parenthesisLevel].isEmpty()){
        if(!performOperation(operand, runningMultiplicativeOper)){
            showError();
            return;
        }
        ui->Display->setText(QString::number(runningFactor[parenthesisLevel]));
    } else {
        runningFactor[parenthesisLevel] = operand;
    }

    runningMultiplicativeOper[parenthesisLevel] = buttonValue;
    awaitingOper[parenthesisLevel] = true;
}


void Calculator::equalsClicked(){
    double operand = ui->Display->text().toDouble();

    if (!runningMultiplicativeOper[parenthesisLevel].isEmpty()) {
        if (!performOperation(operand, runningMultiplicativeOper)) {
            showError();
            return;
        }
        operand = runningFactor[parenthesisLevel];
        runningFactor[parenthesisLevel] = 0.0;
        runningMultiplicativeOper[parenthesisLevel].clear();
    }
    if (!runningAdditiveOper[parenthesisLevel].isEmpty()) {
        if (!performOperation(operand, runningAdditiveOper)) {
            showError();
            return;
        }
        runningAdditiveOper[parenthesisLevel].clear();
    } else {
        runningSum[parenthesisLevel] = operand;
    }

    ui->Display->setText(QString::number(runningSum[parenthesisLevel]));
    runningSum[parenthesisLevel] = 0.0;
    awaitingOper[parenthesisLevel] = true;
}


void Calculator::pointClicked(){
    if (awaitingOper[parenthesisLevel]){
        ui->Display->setText("0");
    }
    if (!ui->Display->text().contains('.')){
            ui->Display->setText(ui->Display->text() + tr("."));
    }
    awaitingOper[parenthesisLevel] = false;
}


void Calculator::changesignClicked()
{
    QString text = ui->Display->text();
    double value = text.toDouble();

    if (value > 0.0) {
        text.prepend(tr("-"));
    } else if (value < 0.0) {
        text.remove(0, 1);
    }
    ui->Display->setText(text);
}


void Calculator::backspaceClicked(){
    if (awaitingOper[parenthesisLevel])
            return;

        QString text = ui->Display->text();
        text.chop(1);
        if (text.isEmpty()) {
            text = "0";
            awaitingOper[parenthesisLevel] = true;
        }
        ui->Display->setText(text);
}


void Calculator::clearClicked(){
    if (awaitingOper[parenthesisLevel]){
        return;
    }
    ui->Display->setText("0");
    awaitingOper[parenthesisLevel] = true;
}


void Calculator::clearallClicked(){
    for(int i = 0; i <= parenthesisLevel; ++i){
        runningSum[i] = 0;
        runningFactor[i] = 0;
        runningAdditiveOper[i].clear();
        runningMultiplicativeOper[i].clear();
        awaitingOper[i] = true;
    }
    ui->Display->setText("0");
}


bool Calculator::performOperation(double rightOperand, const QVector<QString> &pendingOperatorList){
    // Get last operator from list
    QString pendingOperator = pendingOperatorList[parenthesisLevel];

    if (pendingOperator == tr("+")) {
        runningSum[parenthesisLevel] += rightOperand;
    } else if (pendingOperator == tr("-")) {
        runningSum[parenthesisLevel] -= rightOperand;
    } else if (pendingOperator == tr("\303\227")) {  // Multiplication
        runningFactor[parenthesisLevel] *= rightOperand;
    } else if (pendingOperator == tr("\303\267")) {  //Division
        if (rightOperand == 0.0)
            return false;
        runningFactor[parenthesisLevel] /= rightOperand;
    }
    return true;
}


void Calculator::openingparenthesisClicked(){
    parenthesisLevel += 1;
}


void Calculator::closingparenthesisClicked(){
    // Similar to "Equals" button, finishes equation on parenthesisLevel
    double operand = ui->Display->text().toDouble();

    if (!runningMultiplicativeOper[parenthesisLevel].isEmpty()) {
        if (!performOperation(operand, runningMultiplicativeOper)) {
            showError();
            return;
        }
        operand = runningFactor[parenthesisLevel];
        runningFactor[parenthesisLevel] = 0.0;
        runningMultiplicativeOper[parenthesisLevel].clear();
    }
    if (!runningAdditiveOper[parenthesisLevel].isEmpty()) {
        if (!performOperation(operand, runningAdditiveOper)) {
            showError();
            return;
        }
        runningAdditiveOper[parenthesisLevel].clear();
    } else {
        runningSum[parenthesisLevel] = operand;
    }

    ui->Display->setText(QString::number(runningSum[parenthesisLevel]));
    runningSum[parenthesisLevel] = 0.0;
    awaitingOper[parenthesisLevel] = true;
    parenthesisLevel -= 1;
}


void Calculator::showError(){
    for(int i = 0; i <= parenthesisLevel; ++i){
        runningSum[i] = 0;
        runningFactor[i] = 0;
        runningAdditiveOper[i].clear();
        runningMultiplicativeOper[i].clear();
        awaitingOper[i] = true;
    }
    parenthesisLevel = 0;
    ui->Display->setText(QString::number(9999999999));
}
