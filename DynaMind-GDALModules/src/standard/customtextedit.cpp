#include "customtextedit.h"

#include <QtGui>
#include <QAbstractItemView>
#include <QScrollBar>

CustomTextEdit::CustomTextEdit(DM::Module * m, QWidget* parent )
	: QTextEdit( parent )
{
	m_completer = new QCompleter( this );
	m_completer->setWidget( this );

    //QStringList words;
    //words << "working" << "properly";

    std::map<std::string, DM::View> inViews = m->getViewsInStream()["city"];
    QStringList words;

    for (std::map<std::string, DM::View>::const_iterator it = inViews.begin(); it != inViews.end(); ++it) {
        DM::View v = it->second;
        words.append(QString::fromStdString(v.getName()));
    }

	QStringListModel* model = new QStringListModel( words, m_completer );

	m_completer->setModel( model );

	m_completer->setCompletionMode( QCompleter::PopupCompletion );

	connect( m_completer, SIGNAL( activated( QString ) ),
			 this, SLOT( insertCompletion( QString ) ) );
}

void CustomTextEdit::insertCompletion( const QString& completion )
{
	QTextCursor cursor = textCursor();
	int extra = completion.length() - m_completer->completionPrefix().length();
	cursor.movePosition( QTextCursor::Left );
	cursor.movePosition( QTextCursor::EndOfWord );
	cursor.insertText( completion.right( extra ) );
	setTextCursor( cursor );
}

QString CustomTextEdit::textUnderCursor() const
{
	QTextCursor cursor = textCursor();
	cursor.select( QTextCursor::WordUnderCursor );
	return cursor.selectedText();
}

void CustomTextEdit::keyPressEvent( QKeyEvent* event )
{
	if ( m_completer->popup()->isVisible() )
	{

		switch ( event->key() )
		{
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Escape:
		case Qt::Key_Tab:
			event->ignore();
			return;
		}
	}

	QTextEdit::keyPressEvent( event );

	const QString completionPrefix = textUnderCursor();

	if ( completionPrefix != m_completer->completionPrefix() )
	{
		m_completer->setCompletionPrefix( completionPrefix );
		m_completer->popup()->setCurrentIndex( m_completer->completionModel()->index( 0, 0 ) );

	}

    if ( !event->text().isEmpty() && completionPrefix.length() > 1 ) {
        QRect cr = this->cursorRect();
        cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                    + m_completer->popup()->verticalScrollBar()->sizeHint().width());

        m_completer->complete(cr);
    }
}
