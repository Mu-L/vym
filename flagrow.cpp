#include <QDebug>

#include "flagrow.h"

extern bool debug;

/////////////////////////////////////////////////////////////////
// FlagRow
/////////////////////////////////////////////////////////////////
FlagRow::FlagRow()
{
    toolBar=NULL;
    masterRow=NULL;
//    qDebug()<< "Const FlagRow ()";
}

FlagRow::~FlagRow()
{
    //qDebug()<< "Destr FlagRow";
}

Flag* FlagRow::addFlag (Flag *flag) // FIXME-0 switch to Uuids instead of names
{
    Flag *f = new Flag;
;
    f->copy (flag);
    flags.append (f);
    activeNames.append (flag->getName());
    return f;
}

Flag* FlagRow::findFlag (const QString &name)  //FIXME-0 obsolete??
{
    int i = 0;
    while (i <= flags.size() - 1)
    {
	if (flags.at(i)->getName() == name)
	    return flags.at(i);
	i++;	
    }
    return NULL;
}

Flag* FlagRow::findFlag (const QUuid &uid)
{
    int i = 0;
    while (i <= flags.size() - 1)
    {
	if (flags.at(i)->getUuid() == uid)
	    return flags.at(i);
	i++;	
    }
    return NULL;
}

const QStringList FlagRow::activeFlagNames()
{
    return activeNames;
}

const QList <QUuid> FlagRow::activeFlagUids()
{
    qDebug() << "FR::activeFlagUids  " << activeUids << this;
    return activeUids;
}


bool FlagRow::isActive (const QString &name)	
{
    QString n;
    foreach (n, activeNames)
	if (n == name) return true;
    return false;   
}

bool FlagRow::isActive (const QUuid &uid)	
{
    QUuid i;
    foreach (i, activeUids)
	if (i == uid) return true;
    return false;   
}

bool FlagRow::toggle (const QString &name, bool useGroups)  // FIXME-0 obsolete soon
{
    qDebug() << "FRO::toggle  by name " << name  << "active: " << isActive(name);
    if (isActive(name) )
	return deactivate (name);
    else
    {
	if (!activate (name) ) return false;    

	if (!masterRow || !useGroups) return false;

	// Deactivate all other active flags group except "name"
	Flag *flag = masterRow->findFlag (name);
	if (!flag) return false;
	QString mygroup = flag->getGroup();

	for (int i = 0; i < activeNames.size(); ++i)
	{
	    flag = masterRow->findFlag (activeNames.at(i) );
	    if (name != activeNames.at(i) && !mygroup.isEmpty() && mygroup == flag->getGroup())
		deactivate (activeNames.at(i));
	}
	return true;
    }
}

bool FlagRow::toggle (const QUuid &uid, bool useGroups) // FIXME-0 return value not used
{
    qDebug() << "FR::toggle  by uid " << uid.toString();
    if (isActive(uid) )
	return deactivate (uid);
    else
    {
	if (!activate (uid) ) return false;    

	if (!masterRow || !useGroups) return false;

	// Deactivate all other active flags group except "name"
        qDebug() << "FR::toggle  now getting to groups:";
	Flag *flag = masterRow->findFlag (uid);
	if (!flag) return false;
	QString mygroup = flag->getGroup();

	for (int i = 0; i < activeUids.size(); ++i)
	{
	    flag = masterRow->findFlag (activeUids.at(i) );
	    if (uid != activeUids.at(i) && !mygroup.isEmpty() && mygroup == flag->getGroup())
		deactivate (activeUids.at(i));
	}
	return true;
    }
}

bool FlagRow::activate (const QString &name)    // FIXME-0 obsolete
{
    qDebug() << "FR::activate name " << name  << "active: " << isActive(name);
    if (isActive (name)) 
    {
	if (debug) qWarning () << QString("FlagRow::activate - %1 is already active").arg(name);
	return false;
    }

    if (!masterRow)
    {
	qWarning() << "FlagRow::activate - no masterRow to activate " << name;
	return false;
    }

    // Check, if flag exists after all...
    Flag *flag = masterRow->findFlag (name);
    if (!flag)
    {
	qWarning() << "FlagRow::activate - flag " << name << " does not exist here!";
	return false;
    }

    activeNames.append (name);
    return true;
}

bool FlagRow::activate (const QUuid &uid)
{
    qDebug() << "FR::activate uuid " << uid.toString();
    if (isActive (uid)) 
    {
	if (debug) 
            qWarning () << QString("FlagRow::activate - %1 is already active").arg(uid.toString());
	return true;
    }

    if (!masterRow)
    {
	qWarning() << "FlagRow::activate - no masterRow to activate " << uid.toString();
	return false;
    }

    // Check, if flag exists after all...
    qDebug() << "FR::activate   looking in masterRow for flag " << uid;
    Flag *flag = masterRow->findFlag (uid);
    if (!flag)
    {
	qWarning() << "FlagRow::activate - flag " << uid.toString() << " does not exist here!";
	return false;
    }

    activeUids.append (uid);
    qDebug() << "FR::activate   added " << uid << " to " << this;

    return true;
}


bool FlagRow::deactivate (const QString &name) // FIXME-0 obsolete
{
    int n = activeNames.indexOf (name);
    if (n>=0)
    {
	activeNames.removeAt(n);
	return true;
    }
    if (debug) 
	qWarning ()<<QString("FlagRow::deactivate - %1 is not active").arg(name);
    return false;
}

bool FlagRow::deactivate (const QUuid &uid)
{
    int n = activeUids.indexOf (uid);
    if (n >= 0)
    {
	activeUids.removeAt(n);
	return true;
    }
    if (debug) 
	qWarning ()<<QString("FlagRow::deactivate - %1 is not active").arg(uid.toString());
    return true;
}

bool FlagRow::deactivateGroup (const QString &gname) 
{
    if (!masterRow) return false;
    if (gname.isEmpty()) return false;

    foreach (QString s, activeNames )
    {
	Flag *flag=masterRow->findFlag (s);
	if (flag && gname == flag->getGroup())
	    deactivate (s);
    }
    return true;
}

void FlagRow::deactivateAll ()
{
    if (!toolBar) activeNames.clear();
}

void FlagRow::setEnabled (bool b)
{
    toolBar->setEnabled (b);
}

void FlagRow::resetUsedCounter()
{
    for (int i=0; i<flags.size(); ++i)
	flags.at(i)->setUsed (false);
}

QString FlagRow::saveDef()
{
    // For the masterrow of userflags: Write definitions of flags

    QString s = "\n";

    for (int i = 0; i < flags.size(); ++i)
        s += flags.at(i)->saveDef();

    return s;
}

bool FlagRow::saveDataToDir (const QString &tmpdir, const QString &prefix)
{
    bool r = true;
    
    // Save icons to dir, if verbose is set (xml export)
    // and I am a master
    // and this standardflag is really used somewhere.
    // Userflags are written anyway (if master flagrow)
    
    for (int i = 0; i < flags.size(); ++i)
        if (!flags.at(i)->saveDataToDir (tmpdir, prefix))
            r = false;

    return r;	    
}

QString FlagRow::saveState ()
{
    QString s;
    
    if (!activeNames.isEmpty())
        for (int i = 0; i < activeNames.size(); ++i)
        {
            Flag *flag = masterRow->findFlag(activeNames.at(i) );

            // save flag to xml, if flag is set 
            s += flag->saveState();

            // and tell parentRow, that this flag is used   
            flag->setUsed(true);
        }   
    return s;	    
}

void FlagRow::setName (const QString &n)
{
    rowName = n;
}

void FlagRow::setToolBar (QToolBar *tb)
{
    toolBar = tb;
}

void FlagRow::setMasterRow (FlagRow *row)
{
    masterRow = row; 
}

void FlagRow::updateToolBar (QList <QUuid> activeUids)
{
    if (toolBar )
    {
	for (int i = 0;i < flags.size();++i)
	    flags.at(i)->getAction()->setChecked (false);

	for (int i = 0;i < flags.size();++i)
	{
	    int n = activeUids.indexOf (flags.at(i)->getUuid());
	    if (n >= 0)
		flags.at(i)->getAction()->setChecked (true);	
	}
    }
}


