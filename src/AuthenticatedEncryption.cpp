/**
 * Copyright Copyright 2020 BrutalWizard (https://github.com/bru74lw1z4rd). All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution
**/

#include "AuthenticatedEncryption.h"

QSimpleCrypto::AuthenticatedEncryption::AuthenticatedEncryption()
{
}

///
/// \brief QSimpleCrypto::AuthenticatedEncryption::encrypt_aes_gcm
/// \param cipher - can be used with openssl evp chipers (gcm) - 128, 192, 256. Example: EVP_aes_256_gcm()
/// \param data - bytes that will be encrypted
/// \param key
/// \param iv - intilization vector
/// \param aad - (optional) additional authenticated data. If not used, it must equal to nullptr
/// \param tag - auth tag
/// \return
///
QByteArray QSimpleCrypto::AuthenticatedEncryption::encrypt_aes_gcm(QByteArray data, QByteArray key,
    QByteArray iv, QByteArray* tag,
    QByteArray aad, const EVP_CIPHER* cipher)
{
    /* Initialise cipcher */
    std::unique_ptr<EVP_CIPHER_CTX, void (*)(EVP_CIPHER_CTX*)> en { EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free };
    if (en == nullptr) {
        qCritical() << "Couldn't intilise evp cipher. EVP_CIPHER_CTX_new() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Set length of data */
    int plainTextLength = data.size();
    int cipherTextLength = 0;

    /* Initialise cipcher text. Here we will store encrypted data */
    std::unique_ptr<unsigned char[]> ciphertext { new unsigned char[plainTextLength]() };
    if (ciphertext == nullptr) {
        qCritical() << "Couldn't allocate memory for \'ciphertext\'.";
        return QByteArray();
    }

    /* Initialise the encryption operation. */
    if (!EVP_EncryptInit_ex(en.get(), cipher, nullptr, reinterpret_cast<unsigned char*>(key.data()), reinterpret_cast<unsigned char*>(iv.data()))) {
        qCritical() << "Couldn't initialise encryption operation. EVP_EncryptInit_ex() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /*
     * Set IV length if default 12 bytes (96 bits) is not appropriate
    */
    if (!EVP_CIPHER_CTX_ctrl(en.get(), EVP_CTRL_GCM_SET_IVLEN, iv.length(), nullptr)) {
        qCritical() << "Couldn't set IV length. EVP_CIPHER_CTX_ctrl() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Check if we need to use aad */
    if (aad.length() > 0) {
        /*
         * Provide any AAD data. This can be called zero or more times as required
        */
        if (!EVP_EncryptUpdate(en.get(), nullptr, &cipherTextLength, reinterpret_cast<unsigned char*>(aad.data()), aad.length())) {
            qCritical() << "Couldn't provide aad data. EVP_EncryptUpdate() error: " << ERR_error_string(ERR_get_error(), nullptr);
            return QByteArray();
        }
    }

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
    */
    if (!EVP_EncryptUpdate(en.get(), ciphertext.get(), &cipherTextLength, reinterpret_cast<const unsigned char*>(data.data()), plainTextLength)) {
        qCritical() << "Couldn't provide message to be encrypted. EVP_EncryptUpdate() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /*
     * Finalise the encryption. Normally cipher text bytes may be written at
     * this stage, but this does not occur in GCM mode
    */
    if (!EVP_EncryptFinal_ex(en.get(), ciphertext.get(), &plainTextLength)) {
        qCritical() << "Couldn't finalise encryption. EVP_EncryptFinal_ex() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Get the tag */
    if (!EVP_CIPHER_CTX_ctrl(en.get(), EVP_CTRL_GCM_GET_TAG, tag->length(), reinterpret_cast<unsigned char*>(tag->data()))) {
        qCritical() << "Couldn't get tag. EVP_CIPHER_CTX_ctrl() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Finilize data to be readable with qt */
    QByteArray encryptedData = QByteArray(reinterpret_cast<char*>(ciphertext.get()), cipherTextLength);

    return encryptedData;
}

///
/// \brief QSimpleCrypto::AuthenticatedEncryption::decrypt_aes_gcm
/// \param cipher - can be used with openssl evp chipers (gcm) - 128, 192, 256. Example: EVP_aes_256_gcm()
/// \param data - bytes that will be decrypted
/// \param key
/// \param iv - intilization vector
/// \param aad - (optional) additional authenticated data. If not used, it must equal to nullptr
/// \param tag - auth tag
/// \return
///
QByteArray QSimpleCrypto::AuthenticatedEncryption::decrypt_aes_gcm(QByteArray data, QByteArray key,
    QByteArray iv, QByteArray* tag,
    QByteArray aad, const EVP_CIPHER* cipher)
{
    /* Initialise cipcher */
    std::unique_ptr<EVP_CIPHER_CTX, void (*)(EVP_CIPHER_CTX*)> de { EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free };
    if (de.get() == nullptr) {
        qCritical() << "Couldn't intilise evp cipher. EVP_CIPHER_CTX_new() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Set length of data */
    int cipherTextLength = data.size();
    int plainTextLength = 0;

    /* Initialise plain text. Here we will store decrypted data */
    std::unique_ptr<unsigned char[]> plainText { new unsigned char[cipherTextLength]() };
    if (plainText == nullptr) {
        qCritical() << "Couldn't allocate memory for \'plaintext.\'";
        return QByteArray();
    }

    /* Initialise the decryption operation. */
    if (!EVP_DecryptInit_ex(de.get(), cipher, nullptr, reinterpret_cast<unsigned char*>(key.data()), reinterpret_cast<unsigned char*>(iv.data()))) {
        qCritical() << "Couldn't initialise decryption operation. EVP_DecryptInit_ex() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Set IV length. Not necessary if this is 12 bytes (96 bits) */
    if (!EVP_CIPHER_CTX_ctrl(de.get(), EVP_CTRL_GCM_SET_IVLEN, iv.length(), nullptr)) {
        qCritical() << "Couldn't set IV length. EVP_CIPHER_CTX_ctrl() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Check if we need to use aad */
    if (aad.length() > 0) {
        /*
         * Provide any AAD data. This can be called zero or more times as required
        */
        if (!EVP_DecryptUpdate(de.get(), nullptr, &plainTextLength, reinterpret_cast<unsigned char*>(aad.data()), aad.length())) {
            qCritical() << "Couldn't provide aad data. EVP_DecryptUpdate() error: " << ERR_error_string(ERR_get_error(), nullptr);
            return QByteArray();
        }
    }

    /*
     * Provide the message to be decrypted, and obtain the plain text output.
     * EVP_DecryptUpdate can be called multiple times if necessary
    */
    if (!EVP_DecryptUpdate(de.get(), plainText.get(), &plainTextLength, reinterpret_cast<const unsigned char*>(data.data()), cipherTextLength)) {
        qCritical() << "Couldn't provide message to be decrypted. EVP_DecryptUpdate() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Set expected tag value. Works in OpenSSL 1.0.1d and later */
    if (!EVP_CIPHER_CTX_ctrl(de.get(), EVP_CTRL_GCM_SET_TAG, tag->length(), reinterpret_cast<unsigned char*>(tag->data()))) {
        qCritical() << "Coldn't set tag. EVP_CIPHER_CTX_ctrl() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /*
     * Finalise the decryption. A positive return value indicates success,
     * anything else is a failure - the plain text is not trustworthy.
    */
    if (!EVP_DecryptFinal_ex(de.get(), plainText.get(), &cipherTextLength)) {
        qCritical() << "Couldn't finalise decryption. EVP_DecryptFinal_ex() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Finilize data to be readable with qt */
    QByteArray decryptedData = QByteArray(reinterpret_cast<char*>(plainText.get()), plainTextLength);

    return decryptedData;
}

///
/// \brief QSimpleCrypto::AuthenticatedEncryption::encrypt_aes_ccm
/// \param cipher - can be used with openssl evp chipers (ccm) - 128, 192, 256. Example: EVP_aes_256_ccm()
/// \param data - bytes that will be decrypted
/// \param key
/// \param iv - intilization vector
/// \param aad - (optional) additional authenticated data. If not used, it must equal to nullptr
/// \param tag - auth tag
/// \return
///
QByteArray QSimpleCrypto::AuthenticatedEncryption::encrypt_aes_ccm(QByteArray data, QByteArray key,
    QByteArray iv, QByteArray* tag,
    QByteArray aad, const EVP_CIPHER* cipher)
{
    /* Initialise cipcher */
    std::unique_ptr<EVP_CIPHER_CTX, void (*)(EVP_CIPHER_CTX*)> en { EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free };
    if (en == nullptr) {
        qCritical() << "Couldn't intilise evp cipher. EVP_CIPHER_CTX_new() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Set length of data */
    int plainTextLength = data.size();
    int cipherTextLength = 0;

    /* Initialise cipcher text. Here we will store encrypted data */
    std::unique_ptr<unsigned char[]> cipherText { new unsigned char[plainTextLength]() };
    if (cipherText.get() == nullptr) {
        qCritical() << "Couldn't allocate memory for \'ciphertext\'.";
        return QByteArray();
    }

    /* Initialise the encryption operation. */
    if (!EVP_EncryptInit_ex(en.get(), cipher, nullptr, reinterpret_cast<unsigned char*>(key.data()), reinterpret_cast<unsigned char*>(iv.data()))) {
        qCritical() << "Couldn't initialise encryption operation. EVP_EncryptInit_ex() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /*
     * Set IV length if default 12 bytes (96 bits) is not appropriate
    */
    if (!EVP_CIPHER_CTX_ctrl(en.get(), EVP_CTRL_CCM_SET_IVLEN, iv.length(), nullptr)) {
        qCritical() << "Couldn't set IV length. EVP_CIPHER_CTX_ctrl() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Set tag length */
    if (!EVP_CIPHER_CTX_ctrl(en.get(), EVP_CTRL_CCM_SET_TAG, tag->length(), nullptr)) {
        qCritical() << "Coldn't set tag. EVP_CIPHER_CTX_ctrl() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Check if we need to use aad */
    if (aad.length() > 0) {
        /* Provide the total plain text length */
        if (!EVP_EncryptUpdate(en.get(), nullptr, &cipherTextLength, nullptr, plainTextLength)) {
            qCritical() << "Couldn't provide total plaintext length. EVP_EncryptUpdate() error: " << ERR_error_string(ERR_get_error(), nullptr);
            return QByteArray();
        }

        /*
         * Provide any AAD data. This can be called zero or more times as required
        */
        if (!EVP_EncryptUpdate(en.get(), nullptr, &cipherTextLength, reinterpret_cast<unsigned char*>(aad.data()), aad.length())) {
            qCritical() << "Couldn't provide aad data. EVP_EncryptUpdate() error: " << ERR_error_string(ERR_get_error(), nullptr);
            return QByteArray();
        }
    }

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
    */
    if (!EVP_EncryptUpdate(en.get(), cipherText.get(), &cipherTextLength, reinterpret_cast<const unsigned char*>(data.data()), plainTextLength)) {
        qCritical() << "Couldn't provide message to be encrypted. EVP_EncryptUpdate() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /*
     * Finalise the encryption. Normally ciphertext bytes may be written at
     * this stage, but this does not occur in GCM mode
    */
    if (!EVP_EncryptFinal_ex(en.get(), cipherText.get(), &plainTextLength)) {
        qCritical() << "Couldn't finalise encryption. EVP_EncryptFinal_ex() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Get the tag */
    if (!EVP_CIPHER_CTX_ctrl(en.get(), EVP_CTRL_CCM_GET_TAG, tag->length(), reinterpret_cast<unsigned char*>(tag->data()))) {
        qCritical() << "Couldn't get tag. EVP_CIPHER_CTX_ctrl() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Finilize data to be readable with qt */
    QByteArray encryptedData = QByteArray(reinterpret_cast<char*>(cipherText.get()), cipherTextLength);

    return encryptedData;
}

///
/// \brief QSimpleCrypto::AuthenticatedEncryption::decrypt_aes_ccm
/// \param cipher - can be used with openssl evp chipers (ccm) - 128, 192, 256. Example: EVP_aes_256_ccm()
/// \param data - bytes that will be decrypted
/// \param key
/// \param iv - intilization vector
/// \param aad - (optional) additional authenticated data. If not used, it must equal to nullptr
/// \param tag - auth tag
/// \return
///
QByteArray QSimpleCrypto::AuthenticatedEncryption::decrypt_aes_ccm(QByteArray data, QByteArray key,
    QByteArray iv, QByteArray* tag,
    QByteArray aad, const EVP_CIPHER* cipher)
{
    /* Initialise cipcher */
    std::unique_ptr<EVP_CIPHER_CTX, void (*)(EVP_CIPHER_CTX*)> de { EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free };

    if (de.get() == nullptr) {
        qCritical() << "Couldn't intilise evp cipher. EVP_CIPHER_CTX_new() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Set length of data */
    int cipherTextLength = data.size();
    int plainTextLength = 0;

    /* Initialise plain text. Here we will store decrypted data */
    std::unique_ptr<unsigned char[]> plainText { new unsigned char[cipherTextLength]() };
    if (plainText == nullptr) {
        qCritical() << "Couldn't allocate memory for \'plaintext\'.";
        return QByteArray();
    }

    /* Initialise the decryption operation. */
    if (!EVP_DecryptInit_ex(de.get(), cipher, nullptr, reinterpret_cast<unsigned char*>(key.data()), reinterpret_cast<unsigned char*>(iv.data()))) {
        qCritical() << "Couldn't initialise decryption operation. EVP_DecryptInit_ex() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Set IV length. Not necessary if this is 12 bytes (96 bits) */
    if (!EVP_CIPHER_CTX_ctrl(de.get(), EVP_CTRL_CCM_SET_IVLEN, iv.length(), nullptr)) {
        qCritical() << "Couldn't set IV length. EVP_CIPHER_CTX_ctrl() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Set expected tag value. Works in OpenSSL 1.0.1d and later */
    if (!EVP_CIPHER_CTX_ctrl(de.get(), EVP_CTRL_CCM_SET_TAG, tag->length(), reinterpret_cast<unsigned char*>(tag->data()))) {
        qCritical() << "Coldn't set tag. EVP_CIPHER_CTX_ctrl() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Check if we need to use aad */
    if (aad.length() > 0) {
        /* Provide the total ciphertext length */
        if (!EVP_DecryptUpdate(de.get(), nullptr, &plainTextLength, nullptr, cipherTextLength)) {
            qCritical() << "Couldn't provide total plaintext length. EVP_DecryptUpdate() error: " << ERR_error_string(ERR_get_error(), nullptr);
            return QByteArray();
        }

        /*
         * Provide any AAD data. This can be called zero or more times as required
        */
        if (!EVP_DecryptUpdate(de.get(), nullptr, &plainTextLength, reinterpret_cast<unsigned char*>(aad.data()), aad.length())) {
            qCritical() << "Couldn't provide aad data. EVP_DecryptUpdate() error: " << ERR_error_string(ERR_get_error(), nullptr);
            return QByteArray();
        }
    }

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
    */
    if (!EVP_DecryptUpdate(de.get(), plainText.get(), &plainTextLength, reinterpret_cast<const unsigned char*>(data.data()), cipherTextLength)) {
        qCritical() << "Couldn't provide message to be decrypted. EVP_DecryptUpdate() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /*
     * Finalise the decryption. A positive return value indicates success,
     * anything else is a failure - the plaintext is not trustworthy.
    */
    if (!EVP_DecryptFinal_ex(de.get(), plainText.get(), &cipherTextLength)) {
        qCritical() << "Couldn't finalise decryption. EVP_DecryptFinal_ex() error: " << ERR_error_string(ERR_get_error(), nullptr);
        return QByteArray();
    }

    /* Finilize data to be readable with qt */
    QByteArray decryptedData = QByteArray(reinterpret_cast<char*>(plainText.get()), plainTextLength);

    return decryptedData;
}
