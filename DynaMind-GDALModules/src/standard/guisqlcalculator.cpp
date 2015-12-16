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
    ct = new CustomTextEdit(m);
    ct->setText(QString::fromStdString(m->getParameterAsString("query")));
    ui->groupBox_2->layout()->addWidget(ct);

    MyHighlighter *highlighter = new MyHighlighter(ct->document());



}

void GUISQLCalculator::accept()
{
	m->setParameterValue("attribute", ui->lineEditAttributeName->text().toStdString());
    m->setParameterValue("attribute_type", ui->lineEditType->text().toStdString());
    m->setParameterValue("query", ct->toPlainText().toStdString());

	QDialog::accept();
}


GUISQLCalculator::~GUISQLCalculator()
{
	delete ui;
}


MyHighlighter::MyHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{

}

void MyHighlighter::highlightBlock(const QString &text)
{
    std::cout << "highliht" << std::endl;
    QTextCharFormat myClassFormat;
    myClassFormat.setFontWeight(QFont::Bold);
    myClassFormat.setForeground(Qt::darkMagenta);
    QString pattern = "huhu";

    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(expression, index + length);
    }
}

