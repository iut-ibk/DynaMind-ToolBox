#include "guisqlcalculator.h"
#include "ui_guisqlcalculator.h"
#include "customtextedit.h"
#include <QCompleter>


GUISQLCalculator::GUISQLCalculator(DM::Module *m, QWidget *parent) :
	m(m),
	QDialog(parent),
	ui(new Ui::GUISQLCalculator)
{
	ui->setupUi(this);

	ui->lineEditAttributeName->setText(QString::fromStdString(m->getParameterAsString("attribute")));
	ui->lineEditType->setText(QString::fromStdString(m->getParameterAsString("attribute_type")));
	//std::cout << m->getParameterAsString("query") << std::endl;
	//QTextDocument * dt = new QTextDocument();
	//dt->setPlainText(QString::fromStdString(m->getParameterAsString("query")));
	ui->groupBox_2->layout()->addWidget(new CustomTextEdit());
	//ui->plainEditSQLQuery->setPlainText(QString::fromStdString(m->getParameterAsString("query")));

	QStringList wordList;
	wordList << "alpha" << "omega" << "omicron" << "zeta";

	//QLineEdit *lineEdit = new QLineEdit(this);

	QCompleter *completer = new QCompleter(wordList, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	//completer->setWidget(ui->plainEditSQLQuery);
	//ui->plainEditSQLQuery->
	//lineEdit->setCompleter(completer);




}

void GUISQLCalculator::accept()
{
	m->setParameterValue("attribute", ui->lineEditAttributeName->text().toStdString());
	m->setParameterValue("attribute_type", ui->lineEditType->text().toStdString());
//	m->setParameterValue("query", ui->plainEditSQLQuery->document()->toPlainText().toStdString());

	QDialog::accept();
}


GUISQLCalculator::~GUISQLCalculator()
{
	delete ui;
}
