#include "mainwindow.h"
#include "./ui_mainwindow.h"


#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QDebug>
#include <QLabel>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(949, 616);
    connect(ui->b_open, &QPushButton::clicked, this, &MainWindow::open);
    connect(ui->b_save, &QPushButton::clicked, this, &MainWindow::save);
    connect(ui->s_brightness, &QSlider::valueChanged, this, &MainWindow::changeBrightness);
    connect(ui->s_contrast, &QSlider::valueChanged, this, &MainWindow::changeContrast);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Image", "", "Image files (*.png *.jpg *.bmp)");
    if(!filename.isEmpty()){
        qDebug() << "Opening Image" << filename;
    }

    currentImage = QPixmap(filename);

    ui->i_label->setPixmap(currentImage);
}

void MainWindow::changeBrightness(int value)
{
    QImage image = currentImage.toImage();
    image = image.convertToFormat(QImage::Format_ARGB32);

    int brightness = value - 99;
    if (brightness > 0) {
        image.fill(qRgb(brightness, brightness, brightness));
    } else {
        brightness = -brightness;
        for (int y = 0; y < image.height(); ++y) {
            for (int x = 0; x < image.width(); ++x) {
                QRgb color = image.pixel(x, y);
                int r = qRed(color) - brightness;
                int g = qGreen(color) - brightness;
                int b = qBlue(color) - brightness;
                image.setPixel(x, y, qRgb(qMax(0, r), qMax(0, g), qMax(0, b)));
            }
        }
    }

    QPixmap pixmap = QPixmap::fromImage(image);
    ui->i_label->setPixmap(pixmap);
}

void MainWindow::changeContrast(int value)
{
    QImage image = currentImage.toImage();
    image = image.convertToFormat(QImage::Format_ARGB32);

    qreal factor = (qreal)value / 50.0;
    qreal alpha = 1.0;
    qreal beta = 128.0 * (1.0 - factor);

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb color = image.pixel(x, y);
            qreal r = qRed(color) * factor + beta;
            qreal g = qGreen(color) * factor + beta;
            qreal b = qBlue(color) * factor + beta;
            qreal a = qAlpha(color) * alpha;
            image.setPixel(x, y, qRgba(qBound(0.0, r, 255.0), qBound(0.0, g, 255.0), qBound(0.0, b, 255.0), qBound(0.0, a, 255.0)));
        }
    }

    QPixmap pixmap = QPixmap::fromImage(image);
    ui->i_label->setPixmap(pixmap);
}

void MainWindow::save()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Image", "", "Image files (*.png *.jpg *.bmp)");
    if(!filename.isEmpty()){
        qDebug() << "Saving Image" << filename;
        // Get a current image
        QPixmap pixmap = ui->i_label->pixmap().copy();
        // Save image
        pixmap.save(filename);
    }
}

