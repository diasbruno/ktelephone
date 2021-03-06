#include "uamanager.h"

#include <QDebug>
#include <QString>

UserAgentManager::UserAgentManager()
{
  Endpoint *ep = new Endpoint;
  ep->libCreate();

  // Initialize endpoint
  ep->libInit( ep_cfg );

  this->start(5090);
}

UserAgentManager::~UserAgentManager()
{
  //  ep->libDestroy();
  //  delete ep;
}

void UserAgentManager::start(int port)
{
  try {
    // Create SIP transport. Error handling sample is shown
    tcfg.port = port;

    ep->transportCreate(PJSIP_TRANSPORT_UDP, tcfg);

    // Start the library (worker threads etc)
    ep->libStart();

    qDebug() << "*** PJSUA2 STARTED ***";
  } catch (Error &err) {
    qDebug() << "Error starting UserAgentManager";
    this->start(port+1);
    return;
  }
}

AccountConfig UserAgentManager::getAccountConfig(Telephone_t mTelephone)
{
  // Configure an AccountConfig
  AccountConfig acfg;
  QString sip = "sip:";
  QString sipUri = QString();
  acfg.idUri = sipUri.append(sip).append(mTelephone.username).toStdString();
  QString username = QString();
  QString domain = QString();
  domain.append(sip);
  if (!mTelephone.username.split('@').isEmpty()) {
    domain.append(mTelephone.username.split('@').takeLast());
    username.append(mTelephone.username.split('@').takeFirst());
  } else {
    username.append(mTelephone.username);
  }
  acfg.regConfig.registrarUri = domain.toStdString();

  AuthCredInfo cred("digest", "*",  username.toStdString(), 0, mTelephone.password.toStdString());
  acfg.sipConfig.authCreds.push_back( cred );
  return acfg;
}

void UserAgentManager::newUserAgent(KTelephone* telephone, QString username, AccountConfig acfg)
{
  UserAgent *acc = new UserAgent;
  try {
    acc->setInstance(telephone);
    acc->create(acfg);
    mAccounts[username] = acc;
  } catch(Error& err) {
    qDebug() << "Account creation error";
    delete acc;
  }
}

void UserAgentManager::removeUserAgent(QString username)
{
  if (mAccounts.contains(username)) {
    // delete Account in order to unregister
    delete mAccounts.value(username);
    mAccounts.remove(username);
  }
}

void UserAgentManager::setRegister(QString username, bool status)
{
  if(mAccounts[username]) {
    mAccounts[username]->setRegistration(status);
  }
}
