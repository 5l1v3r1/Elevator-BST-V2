#include "stoform.h"
#include "ui_stoform.h"

StoForm::StoForm(QString pFilePath, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StoForm)
{
    ui->setupUi(this);

    setupModel();
    setupViews();

    openFile(pFilePath);
}

StoForm::~StoForm()
{
    delete ui;
}

void StoForm::setupModel()
{
    model = new QStandardItemModel(8, 2, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Label"));
    model->setHeaderData(1, Qt::Horizontal, tr("Quantity"));
}

void StoForm::setupViews()
{
    QSplitter *splitter = new QSplitter;
    QTableView *table = new QTableView;
    pieChart = new PieView;
    splitter->addWidget(table);
    splitter->addWidget(pieChart);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    table->setModel(model);
    pieChart->setModel(model);

    QItemSelectionModel *selectionModel = new QItemSelectionModel(model);
    table->setSelectionModel(selectionModel);
    pieChart->setSelectionModel(selectionModel);

    QHeaderView *headerView = table->horizontalHeader();
    headerView->setStretchLastSection(true);

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(splitter);
}

void StoForm::openFile(const QString &path)
{
    QString fileName;
    if (path.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Choose a data file"),
                                                "", "*.cht");
    else
        fileName = path;

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&file);
            QString line;

            model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());

            int row = 0;
            do {
                line = stream.readLine();
                if (!line.isEmpty()) {

                    model->insertRows(row, 1, QModelIndex());

                    QStringList pieces = line.split(",", QString::SkipEmptyParts);
                    model->setData(model->index(row, 0, QModelIndex()),
                                   pieces.value(0));
                    model->setData(model->index(row, 1, QModelIndex()),
                                   pieces.value(1));
                    model->setData(model->index(row, 0, QModelIndex()),
                                   QColor(pieces.value(2)), Qt::DecorationRole);
                    row++;
                }
            } while (!line.isEmpty());

            file.close();
            GlobalPara.showMessage(tr("Loaded %1").arg(fileName));
        }
    }
}

void StoForm::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save file as"), "", "*.cht");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        QTextStream stream(&file);

        if (file.open(QFile::WriteOnly | QFile::Text)) {
            for (int row = 0; row < model->rowCount(QModelIndex()); ++row) {

                QStringList pieces;

                pieces.append(model->data(model->index(row, 0, QModelIndex()),
                                          Qt::DisplayRole).toString());
                pieces.append(model->data(model->index(row, 1, QModelIndex()),
                                          Qt::DisplayRole).toString());
                pieces.append(model->data(model->index(row, 0, QModelIndex()),
                                          Qt::DecorationRole).toString());

                stream << pieces.join(",") << "\n";
            }
        }

        file.close();
        GlobalPara.showMessage(tr("Saved %1").arg(fileName));
    }
}

