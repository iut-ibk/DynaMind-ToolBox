#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QTextEdit>
#include <QCompleter>
#include "dmmodule.h"

class QCompleter;

class CustomTextEdit : public QTextEdit
{
	Q_OBJECT

public:
    CustomTextEdit(DM::Module *m, QWidget* parent = 0 );

private slots:
	void insertCompletion( const QString& completion );

private:
	void keyPressEvent( QKeyEvent* event );
	QString textUnderCursor()const;

	QCompleter* m_completer;
};

#endif // CUSTOMTEXTEDIT_H
