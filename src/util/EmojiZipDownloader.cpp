#include "EmojiZipDownloader.h"
#include <QMessageBox>

EmojiZipDownloader::EmojiZipDownloader(QNetworkAccessManager* manager, QString downloadUrl, QString outputPath) {
    this->outputPath = outputPath;

    // temporary files only get a filename once they have
    // been opened once
    zipFile.open();
    zipFile.close();

    downloader = new FileDownloader(manager, downloadUrl, zipFile.fileName());
    downloader->dontDisplayErrorMessages();

    connect(
        downloader, &FileDownloader::finished,
        this, &EmojiZipDownloader::downloaded
    );

    connect(
        downloader, &FileDownloader::IOError,
        this, [&]() { onError(EmojiZipError::IOError); }
    );

    connect(
        downloader, &FileDownloader::error,
        this, [&](QNetworkReply::NetworkError error) { onError(EmojiZipError::NetworkError); }
    );
}

EmojiZipDownloader::~EmojiZipDownloader() {
    delete downloader;
    delete unzipper;
}

void EmojiZipDownloader::downloadAndExtract() {
    downloader->start();
}

void EmojiZipDownloader::downloaded() {
    delete unzipper;

    unzipper = new Unzipper(zipFile.fileName());
    bool result = unzipper->unzipAllFilesToPath(outputPath);

    if (!result) {
        onError(EmojiZipError::UnarchivingError);
    } else {
        emit done();
    }
}

void EmojiZipDownloader::onError(EmojiZipError cause) {
    emit failed();

    QString message = "Encountered error while ";

    switch (cause) {
        case EmojiZipError::IOError:
            message += "creating files.";
            break;
        case EmojiZipError::NetworkError:
            message += "downloading the emoji pack.";
            break;
        case EmojiZipError::UnarchivingError:
            message += "decompressing the emojis.";
            break;
    }

    QMessageBox::critical(nullptr, "Failed to download emojis!", message);
}
