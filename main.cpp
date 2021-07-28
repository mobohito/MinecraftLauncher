#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QDir>
#include <QCryptographicHash>

class downloader{

public:
    QString path;
    QNetworkReply *reply;
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QSslConfiguration config;
    QFile *downloadFile;
    QUrl url = QUrl::fromUserInput("");

    downloader(QString path,QString url, QString sha1);

};


void autocrteatedir(QString path){
    int count = path.count("/");
    for (int i=1;i<=count-1;i++){

        //qDebug()<<path.section("/",0,i);
        QString usefulPATH = path.section("/",0,i);

        QDir dir;
        //qDebug()<<dir.exists(usefulPATH);
        if  (!dir.exists(usefulPATH)){
            dir.mkdir(usefulPATH);
        }
    }
}


QString getsha1(QFile *file){
    file->open(QIODevice::ReadOnly);
    QByteArray ba = QCryptographicHash::hash(file->readAll(), QCryptographicHash::Sha1);

    file->close();
    return QString(ba.toHex());
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QString root = "C:/Users/ms226/Desktop/.minecraft";
    QFile json("C:/Users/ms226/Desktop/1.16.5.json");
    if (!json.open(QIODevice::ReadOnly)){
        qDebug()<<"Json file cannot be opened";
    }
    QFile save("C:/Users/ms226/Desktop/1.txt");
    if (!save.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug()<<"Cannot create save file";
    }

    QByteArray jsonarr = json.readAll();
    QJsonDocument jsondoc = QJsonDocument::fromJson(jsonarr);
    QJsonObject jsonobj = jsondoc.object();
    QJsonValue libval = jsonobj.take("libraries");
    QJsonObject obj = libval.toObject();
    QJsonArray libarr = libval.toArray();

    for (int i=0;i<libarr.size();i++){
        QJsonObject insideobj = libarr.at(i).toObject();

        QJsonValue insideval = insideobj.take("downloads");
        QJsonObject downObj = insideval.toObject();

        QJsonValue artiVal = downObj.take("artifact");
        QJsonObject artiObj = artiVal.toObject();

        QString path = artiObj.take("path").toString();
        QString url = artiObj.take("url").toString();
        QString sha1 = artiObj.take("sha1").toString();

        QString jueduilujing = root + "/libraries/" + path;

        //qDebug()<<jueduilujing;
        //qDebug()<<url;

        QFile *libFile = new QFile(jueduilujing);
        bool filexise = libFile->exists();

        if (!filexise){

            downloader *download = new downloader(jueduilujing,url,sha1);
            download = Q_NULLPTR;
        }
        else {
            if (sha1 != getsha1(libFile)){
                qDebug()<<"The File" + libFile->fileName() + "sha1 is not fit";
                libFile->remove();

                downloader *download = new downloader(jueduilujing,url, sha1);
                download = Q_NULLPTR;
            }
        }

        //qDebug()<<jueduilujing;
        QByteArray output = jueduilujing.toUtf8();
        save.write(output);

    }
    save.close();
    json.close();
    return a.exec();
}


downloader::downloader(QString path,QString url, QString sha1){
    int a = path.lastIndexOf("/");
        QDir dir;
        if (!dir.exists(path.left(a))){
            autocrteatedir(path);
        }

    downloadFile = new QFile(path);
    if (!downloadFile->open(QIODevice::WriteOnly)){
          qDebug()<<"Cannot write file";
          return;
    }

    config = request.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    request.setSslConfiguration(config);
    request.setUrl(QUrl(url));

    reply = manager.get(request);


    QObject::connect(reply,&QNetworkReply::finished,[=]()->void{
        downloadFile->close();
        qDebug()<<"download finished";

    });


    QObject::connect(reply,&QNetworkReply::readyRead,[=]()->void{
        qDebug()<<"Downloading";
        downloadFile->write(reply->readAll());
    });
}
