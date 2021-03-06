#include "RecordMng.h"
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>
CRecordMng* CRecordMng::g_Inst=NULL;
CRecordMng::CRecordMng(QObject*p):
QObject(p)
{
	
#ifndef Q_OS_WIN
	m_strLogFile = QString("/home/ns5000/data/InsituSys1.log");
#else
	m_strLogFile = QString("InsituSys1.log");
#endif

	m_fFile = NULL;
	m_pSizeMngTimer = NULL;
	m_pWriteTimer = NULL;
	ReadIni();
	if(m_bCreateLog) //是否生成log文件
	{
		m_fFile = new QFile(m_strLogFile);

		m_pSizeMngTimer = new QTimer(this);
		m_pSizeMngTimer->setInterval(m_nCheckTimeSpan*1000);
		connect(m_pSizeMngTimer,SIGNAL(timeout()),this,SLOT(ManageFileSizeSlot()));

		m_pWriteTimer = new QTimer(this);
		m_pWriteTimer->setInterval(m_nWriteLogSpan*1000);
		connect(m_pWriteTimer,SIGNAL(timeout()),this,SLOT(WriteLogSlot()));	
		
		m_pSizeMngTimer->start();
		m_pWriteTimer->start();
	}

}

void CRecordMng::ReadIni()
{
#ifdef Q_OS_WIN
	const QString strDocDir = QCoreApplication::applicationDirPath();
#else
	const QString strDocDir = "/home/ns5000/config/InsituSys/";
#endif

	QDir docDir(strDocDir);
	if ( !docDir.exists())
	{
		QDir().mkpath( strDocDir );
	}

	const QString strDbgIni = strDocDir + "/pi103.ini";
	if ( !QFile::exists( strDbgIni ) )
	{
		m_bCreateLog = true;
		m_bDbg = true;
		m_nFileMaxSize = 100;
	}

	QSettings *ini = new QSettings( strDbgIni, QSettings::IniFormat );

	ini->beginGroup("LogSetting");
	m_bDbg = ini->value("DebugEnabled", true).toBool();
	m_bCreateLog = ini->value("LogEnabled",true).toBool();
	m_nFileMaxSize  = ini->value("LogMaxSize",100).toInt();
	m_nCheckTimeSpan = ini->value("CheckTime",60*60).toInt();
	m_nWriteLogSpan = ini->value("WriteTime",1).toInt();
	//不允许用户设置无效数值，大于1g或者小于10M都不允许
	if(m_nFileMaxSize<1||m_nFileMaxSize>1000)
	{
		m_nFileMaxSize = 100;
	}
	//小于10s或者超过一天都属于无效数据
	if(m_nCheckTimeSpan<10||m_nCheckTimeSpan>60*60*24)
	{
		m_nCheckTimeSpan = 60*60;
	}
	if(m_nWriteLogSpan<1||m_nWriteLogSpan>10)
	{
		m_nWriteLogSpan = 5;
	}

	ini->endGroup();
	delete ini;
}



void CRecordMng::OutputMsg(QByteArray arry)
{
	QDateTime timeTmp = QDateTime::currentDateTime();
	QByteArray strTime = timeTmp.toString(" yyyy-MM-dd hh:mm:ss zzz ").toLocal8Bit();
	QByteArray str = strTime+arry+("\r\n");
	if(m_bCreateLog)
	{
		//m_Mtx.lock();
		//m_fFile->write(str.toAscii());
		m_strContentLst.append(str);		
		//m_Mtx.unlock();
	}
	if(m_bDbg)
	{
		qDebug()<<str;
	}
}

void CRecordMng::ManageFileSizeSlot()
{
	QDateTime dt1 = QDateTime::currentDateTime();
	int nSize = m_fFile->size();
	if(nSize>m_nFileMaxSize*1024*1024/2)
	{
		m_Mtx.lock();
		//缩小一半
		m_fFile->close();

#ifndef Q_OS_WIN
		if(m_strLogFile==QString("/home/ns5000/data/InsituSys1.log"))
		{			
			m_strLogFile = QString("/home/ns5000/data/InsituSys2.log");
			if(QFile::exists(m_strLogFile))
			{
				QFile::remove(m_strLogFile);
			}
			m_fFile->setFileName(m_strLogFile);
		}
		else
		{
			QFile::remove(m_strLogFile);
			m_strLogFile = QString("/home/ns5000/data/InsituSys1.txt");
			if(QFile::exists(m_strLogFile))
			{
				QFile::remove(m_strLogFile);
			}
			m_fFile->setFileName(m_strLogFile);
		}
		
#else
		if(m_strLogFile == QString("InsituSys1.log"))
		{
			m_strLogFile = QString("InsituSys2.log");
			if(QFile::exists(m_strLogFile))
			{
				QFile::remove(m_strLogFile);
			}
			m_fFile->setFileName(m_strLogFile);
		}
		else
		{
			m_strLogFile = QString("InsituSys1.log");
			if(QFile::exists(m_strLogFile))
			{
				QFile::remove(m_strLogFile);
			}
			m_fFile->setFileName(m_strLogFile);
		}
#endif
	/*	if(!m_fFile->open(QIODevice::ReadWrite))
		{
			m_bCreateLog = false;
			m_bDbg = true;
			m_Mtx.unlock();
			return ;
		}
		QList<QByteArray> lstContent;
		int nLineCount = 0;
		while(!m_fFile->atEnd())
		{
			QByteArray strContent = m_fFile->readLine();
			lstContent.append(strContent);
			nLineCount++;

		}
		m_fFile->close();
		m_fFile->remove(m_strLogFile);
		//删掉三分之一吧
		int nRmvCount = 0;
		while(nRmvCount<nLineCount/3)
		{
			lstContent.removeFirst();
			if(nRmvCount++%100==0)
			{
				QCoreApplication::processEvents();
			}
		}
		
		if(m_fFile->open(QIODevice::WriteOnly))
		{
			int n = 0;
			foreach(QByteArray arry,lstContent)
			{
				m_fFile->write(arry);

				if(n++%100==0)
				{
					QCoreApplication::processEvents();
				}
			}
		}
		QDateTime dt2 = QDateTime::currentDateTime();
		QString strCost = QString("ManageFileSizeSlot cost: %1 s %2 ms").arg(dt1.secsTo(dt2)).arg(dt1.msecsTo(dt2));
		m_fFile->write(strCost.toAscii());
		m_fFile->close();*/
		
		
		m_Mtx.unlock();
	}
}

void CRecordMng::WriteLogSlot()
{	
	m_Mtx.lock();
	if(m_fFile->open(QIODevice::WriteOnly|QIODevice::Append))
	{
		foreach(QString strContent,m_strContentLst)
		{
			m_fFile->write(strContent.toAscii());
		}
		m_strContentLst.clear();
	}
	m_fFile->close();
	m_Mtx.unlock();
}

CRecordMng::~CRecordMng()
{
	if(m_bCreateLog)
	{
		m_pSizeMngTimer->stop();
		m_pWriteTimer->stop();
		WriteLogSlot();
		m_fFile->close();
		delete m_fFile;
	}
};

CRecordMng* CRecordMng::getInst()
{
	if(g_Inst==NULL)
	{
		g_Inst = new CRecordMng;
	}
	return g_Inst;
}




