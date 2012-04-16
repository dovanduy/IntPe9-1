#include "Main.h"

MainGui* gui;

MainGui::MainGui(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
//As debug i run from compiler so change the path to the output path of all bins
#ifdef _DEBUG
	QDir::setCurrent("../../bin/VC100_Debug");
#endif

	gui = this;

	_mainView.setupUi(this);

	//Add this running directory to the path environment (for custom dll's, python)
	QSettings environment("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", QSettings::NativeFormat);
	QStringList paths = environment.value("Path").toString().split(';');
	if(!paths.contains(QDir::currentPath(), Qt::CaseInsensitive))
	{
		paths.append(QDir::currentPath());
		environment.setValue("Path", paths.join(";"));
	}

	//Create all sub views
	_aboutGui = new AboutGui(this);

	//Docks
	_cores = new Cores(&_mainView);
	_mainView.tableCores->setModel(_cores);
	_mainView.tableCores->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	_mainView.tableCores->horizontalHeader()->resizeSection(0, 45);
	_mainView.tableCores->horizontalHeader()->resizeSection(1, 50);
	_mainView.tableCores->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	_cores->readCores("Cores");

	//Other initializing
	//_packetList = new PacketList();
	//_mainView.packetList->setModel(_packetList);

	_hexView = new QHexEdit(_mainView.widget);
	_hexView->setReadOnly(true);
	_mainView.hexWidget->layout()->addWidget(_hexView);

	//Custom GUI setup

	connect(_mainView.actionAbout, SIGNAL(triggered()), _aboutGui, SLOT(slotShow()));
	connect(_mainView.actionSavePackets, SIGNAL(triggered()), this, SLOT(saveAllAsText()));
	connect(_mainView.actionClear_packet_list, SIGNAL(triggered()), this, SLOT(clearList()));
	
	//

	qRegisterMetaType<Sniffer*>("Sniffer*");
}

MainGui::~MainGui()
{
	//Delete this dir from path environment
	QSettings environment("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", QSettings::NativeFormat);
	QStringList paths = environment.value("Path").toString().split(';');
	if(paths.removeAll(QDir::currentPath()) > 0)
		environment.setValue("Path", paths.join(";"));
}

void MainGui::clearList()
{
	_packetList->clear();
	_hexView->setData(NULL);
}

void MainGui::registerPacketView(Sniffer *sniffer)
{
	sniffer->buildGui();
	_mainView.tabPackets->addTab(sniffer->getView(), sniffer->getCore()->getExeName());

	//Change some settings in the layout
	connect(sniffer->packetView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(slotOnClickPacketList(const QModelIndex &, const QModelIndex &)));

}

void MainGui::saveAllAsText()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Save all packets"), NULL, tr("Packets (*.pacs)"));

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return;
	}

	QTextStream out(&file);

	for(int32 i = 0; i < _packetList->rowCount(); i++)
	{
		Packet *packet = _packetList->getPacketAt(i);

		//Format it
		out << packet->strInfoHeader();
		out << packet->strFullDump();
		out << "\n";
	}

	file.close();
}

void MainGui::slotOnClickPacketList(const QModelIndex &current, const QModelIndex &previous)
{
	PacketList *model = (PacketList*)current.model();
	Packet *packet = model->getPacketAt(current.row());

	_hexView->setData(*packet->getData());
}

void MainGui::slotShow()
{
	show();
}

void MainGui::slotHide()
{
	hide();
}

Ui::mainView *MainGui::getView()
{
	return &_mainView;
}
