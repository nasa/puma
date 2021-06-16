#ifndef Q_DEBUGSTREAM_H
#define Q_DEBUGSTREAM_H


#include <iostream>
#include <streambuf>
#include <string>
#include <QScrollBar>
#include "QTextEdit"
#include "QDateTime"


// MessageHandler
 /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
class MessageHandler : public QObject
{
 Q_OBJECT
    public :
        MessageHandler(QTextEdit *textEdit, QObject * parent = Q_NULLPTR) : QObject(parent), m_textEdit(textEdit){}

    public slots:
        void catchMessage(QString msg)
        {
           this->m_textEdit->append(msg);
        }
        /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    private:
        QTextEdit * m_textEdit;
};
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


class ThreadLogStream : public QObject, public std::basic_streambuf<char>
{
Q_OBJECT
public:
    ThreadLogStream(std::ostream &stream, QObject * parent = Q_NULLPTR) :QObject(parent), m_stream(stream)
    {
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }
    ~ThreadLogStream()
    {
        // output anything that is left
        if (!m_string.empty())
        {
        emit sendLogString(QString::fromStdString(m_string));
        }
        m_stream.rdbuf(m_old_buf);
        }
protected:
    virtual int_type overflow(int_type v)
    {
        if (v == '\n')
        {
            emit sendLogString(QString::fromStdString(m_string));
            m_string.erase(m_string.begin(), m_string.end());
        }
        else
            m_string += v;
        return v;
    }
    virtual std::streamsize xsputn(const char *p, std::streamsize n)
    {
        m_string.append(p, p + n);
        long pos = 0;
        while (pos != static_cast<long>(std::string::npos))
        {
            pos = static_cast<long>(m_string.find('\n'));
            if (pos != static_cast<long>(std::string::npos))
            {
                std::string tmp(m_string.begin(), m_string.begin() + pos);
                emit sendLogString(QString::fromStdString(tmp));
                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }
        return n;
    }
private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
signals:
    void sendLogString(const QString& str);
};


#endif // Q_DEBUGSTREAM_H
