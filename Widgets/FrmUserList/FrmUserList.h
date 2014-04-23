#ifndef FRMUSERLIST_H
#define FRMUSERLIST_H

#include <QFrame>
#include <QString>
#include <QStandardItemModel>
#include <QMap>
#include "qxmpp/QXmppRosterIq.h"
#include "qxmpp/QXmppPresence.h"
#include "qxmpp/QXmppVCardIq.h"
#include "qxmpp/QXmppLogger.h"
#include "qxmpp/QXmppMessage.h"
#include "../FrmAddRoster/FrmAddRoster.h"
#include "TreeUserList.h"

class CRoster;

namespace Ui {
class CFrmUserList;
}

class MainWindow;
class CFrmUserList : public QFrame
{
    Q_OBJECT

public:
    explicit CFrmUserList(QWidget *parent = 0);
    ~CFrmUserList();

private:
    //向用户列表中插入用户
    int InsertUser(QXmppRosterIq::Item rosterItem);
    int UpdateGroup(CRoster* pRoster, QSet<QString> groups);

    void resizeEvent(QResizeEvent *);

    //显示上下文件菜单(右键菜单)
    //控件contextMenuPolicy属性要设置为DefaultContextMenu，才能触这个事件
    //如果设置为CustomContextMenu，触发槽on_tvUsers_customContextMenuRequested
    void contextMenuEvent(QContextMenuEvent *);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent * event);

    //得到树形控件当前选中的好友指针
    //如果为空,则为组
    CRoster *GetCurrentRoster();

    QSet<QString> GetGroupsName();

protected slots:
    //好友列表
    void rosterReceived();
    //好友出席状态改变时触发
    void ChangedPresence(const QXmppPresence &presence);

    void vCardReceived(const QXmppVCardIq&);

    //当有好友请求订阅时触发，在这个函数中调用acceptSubscription()接受请求，
    //请用refuseSubscription()拒绝请求
    //如果设置QXmppConfiguration::autoAcceptSubscriptions(),则QXmpp直接接受请求,不触发本信号
    void subscriptionReceived(const QString& bareJid);
    //当一个好友实体订阅时触发(即好友增加)
    void itemAdded(const QString& bareJid);
    // 好友实体订阅发生改变时触发(即好友关系发生改变)
    void itemChanged(const QString& bareJid);
    //删除订阅时触发(即删除好友)
    void itemRemoved(const QString& bareJid);

    //接收好友消息
    void clientMessageReceived(const QXmppMessage &message);

    //控件
    void clicked(const QModelIndex & index);
    void doubleClicked(const QModelIndex & index);

private slots:
    //显示上下文件菜单(右键菜单)
    //控件contextMenuPolicy属性要设置为CustomContextMenu，才能触这个槽
    //如果设置为DefaultContextMenu，则触发右键菜单事件contextMenuEvent()
    void on_tvUsers_customContextMenuRequested(const QPoint &pos);
    //增加好友订阅
    void slotAddRoster();
    //从好友列表中同间增加此好友订阅
    void slotAgreeAddRoster();
    //删掉好友订阅
    void slotRemoveRoster();

private:
    Ui::CFrmUserList *ui;

    CTreeUserList m_UserList;
    QStandardItemModel *m_pModel;           //好友列表树型控件
    MainWindow *m_pMainWindow;
    QMap<QString, CRoster*> m_Rosters;      //好友列表
    QMap<QString, QStandardItem*> m_Groups; //组列表

    CFrmAddRoster m_frmAddRoster;           //增加好友对话框
};

#endif // FRMUSERLIST_H
