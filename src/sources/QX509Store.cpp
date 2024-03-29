/*
 * Copyright 2023 BrutalWizard (https://github.com/bru74lw1z4rd). All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution
 */

#include "include/QX509Store.h"

QSimpleCrypto::QX509Store::QX509Store()
{
}

///
/// \brief QSimpleCrypto::QX509::addCertificateToStore - Function adds X509 certificate to X509 store.
/// \param store - OpenSSL X509_STORE.
/// \param x509 - OpenSSL X509 certificate that will be added to store.
/// \return Returns 'true' on success or "false" on failure.
///
bool QSimpleCrypto::QX509Store::addCertificateToStore(X509_STORE* store, X509* x509)
{
    try {
        if (!X509_STORE_add_cert(store, x509)) {
            throw std::runtime_error("Couldn't add certificate to X509_STORE. X509_STORE_add_cert(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        return true;
    } catch (const std::runtime_error& exception) {
        std::throw_with_nested(exception);
    } catch (...) {
        throw false;
    }
}

///
/// \brief QSimpleCrypto::QX509Store::addLookup - Function adds lookup method for X509 store.
/// \param store - OpenSSL X509_STORE.
/// \param method - OpenSSL X509_LOOKUP_METHOD. Example: X509_LOOKUP_file.
/// \return Returns 'true' on success or "false" on failure.
///
bool QSimpleCrypto::QX509Store::addLookup(X509_STORE* store, X509_LOOKUP_METHOD* method)
{
    try {
        if (!X509_STORE_add_lookup(store, method)) {
            throw std::runtime_error("Couldn't add lookup to X509_STORE. X509_STORE_add_lookup(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        return true;
    } catch (const std::runtime_error& exception) {
        std::throw_with_nested(exception);
    } catch (...) {
        throw false;
    }
}

///
/// \brief QSimpleCrypto::QX509Store::setDepth - Function sets store for X509 store.
/// \param store - OpenSSL X509_STORE.
/// \param depth - That is the maximum number of untrusted CA certificates that can appear in a chain. Example: 0.
/// \return Returns 'true' on success or "false" on failure.
///
bool QSimpleCrypto::QX509Store::setDepth(X509_STORE* store, const quint32 depth)
{
    try {
        if (!X509_STORE_set_depth(store, depth)) {
            throw std::runtime_error("Couldn't set depth for X509_STORE. X509_STORE_set_depth(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        return true;
    } catch (const std::runtime_error& exception) {
        std::throw_with_nested(exception);
    } catch (...) {
        throw false;
    }
}

///
/// \brief QSimpleCrypto::QX509Store::setFlag - Function sets flag for X509 store.
/// \param store - OpenSSL X509_STORE.
/// \param flag - The verification flags consists of zero or more of the following flags ored together. Example: X509_V_FLAG_CRL_CHECK.
/// \return Returns 'true' on success or "false" on failure.
///
bool QSimpleCrypto::QX509Store::setFlag(X509_STORE* store, const quint32 flag)
{
    try {
        if (!X509_STORE_set_flags(store, flag)) {
            throw std::runtime_error("Couldn't set flag for X509_STORE. X509_STORE_set_flags(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        return true;
    } catch (const std::runtime_error& exception) {
        std::throw_with_nested(exception);
    } catch (...) {
        throw false;
    }
}

///
/// \brief QSimpleCrypto::QX509Store::setPurpose - Function sets purpose for X509 store.
/// \param store - OpenSSL X509_STORE.
/// \param purpose - Verification purpose in param to purpose. Example: X509_PURPOSE_ANY.
/// \return Returns 'true' on success or "false" on failure.
///
bool QSimpleCrypto::QX509Store::setPurpose(X509_STORE* store, const quint8 purpose)
{
    try {
        if (!X509_STORE_set_purpose(store, purpose)) {
            throw std::runtime_error("Couldn't set purpose for X509_STORE. X509_STORE_set_purpose(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        return true;
    } catch (const std::runtime_error& exception) {
        std::throw_with_nested(exception);
    } catch (...) {
        throw false;
    }
}

///
/// \brief QSimpleCrypto::QX509Store::setTrust - Function sets trust level for X509 store.
/// \param store - OpenSSL X509_STORE.
/// \param trust - Trust Level. Example: X509_TRUST_SSL_SERVER.
/// \return Returns 'true' on success or "false" on failure.
///
bool QSimpleCrypto::QX509Store::setTrust(X509_STORE* store, const quint8 trust)
{
    try {
        if (!X509_STORE_set_trust(store, trust)) {
            throw std::runtime_error("Couldn't set trust for X509_STORE. X509_STORE_set_trust(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        return true;
    } catch (const std::runtime_error& exception) {
        std::throw_with_nested(exception);
    } catch (...) {
        throw false;
    }
}

///
/// \brief QSimpleCrypto::QX509Store::loadStoreDefaultCertificates - Function loads certificates into the X509_STORE from the hardcoded default paths.
/// \param store - OpenSSL X509_STORE.
/// \return Returns 'true' on success or "false" on failure.
///
bool QSimpleCrypto::QX509Store::loadStoreDefaultCertificates(X509_STORE* store)
{
    try {
        if (!X509_STORE_set_default_paths(store)) {
            throw std::runtime_error("Couldn't set default paths for X509_STORE. X509_STORE_set_default_paths(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        return true;
    } catch (const std::runtime_error& exception) {
        std::throw_with_nested(exception);
    } catch (...) {
        throw false;
    }
}

///
/// \brief QSimpleCrypto::QX509Store::loadLocations - Load locations for X509 store.
/// \param store - OpenSSL X509_STORE.
/// \param fileName - File name. Example: "caCertificate.pem".
/// \param dirPath - Path to file. Example: "root/etc".
/// \return Returns 'true' on success or "false" on failure.
///
bool QSimpleCrypto::QX509Store::loadLocations(X509_STORE* store, const QByteArray& fileName, const QByteArray& dirPath)
{
    try {
        /* Initialize QFileInfo to read information about file */
        QFileInfo fileInfo(dirPath + "/" + fileName);

        if (fileInfo.exists()) {
            if (!X509_STORE_load_locations(store, fileName, dirPath)) {
                throw std::runtime_error("Couldn't load locations for X509_STORE. X509_STORE_load_locations(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
            }

            return true;
        }

        return false;
    } catch (const std::runtime_error& exception) {
        std::throw_with_nested(exception);
    } catch (...) {
        throw false;
    }
}

///
/// \brief QSimpleCrypto::QX509Store::loadLocations - Load locations for X509 store.
/// \param store - OpenSSL X509_STORE.
/// \param file - Qt QFile not null object.
/// \return Returns 'true' on success or "false" on failure.
///
bool QSimpleCrypto::QX509Store::loadLocations(X509_STORE* store, const QFile& file)
{
    try {
        /* Initialize QFileInfo to read information about file */
        QFileInfo fileInfo(file);

        if (fileInfo.exists()) {
            if (!X509_STORE_load_locations(store, fileInfo.fileName().toLocal8Bit(), fileInfo.absoluteDir().path().toLocal8Bit())) {
                throw std::runtime_error("Couldn't load locations for X509_STORE. X509_STORE_load_locations(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
            }

            return true;
        }

        return false;
    } catch (const std::runtime_error& exception) {
        std::throw_with_nested(exception);
    } catch (...) {
        throw false;
    }
}

///
/// \brief QSimpleCrypto::QX509Store::loadLocations - Load locations for X509 store.
/// \param store - OpenSSL X509_STORE.
/// \param fileInfo - Qt QFileInfo not null object.
/// \return Returns 'true' on success or "false" on failure.
///
bool QSimpleCrypto::QX509Store::loadLocations(X509_STORE* store, const QFileInfo& fileInfo)
{
    try {
        if (fileInfo.exists()) {
            if (!X509_STORE_load_locations(store, fileInfo.fileName().toLocal8Bit(), fileInfo.absoluteDir().path().toLocal8Bit())) {
                throw std::runtime_error("Couldn't load locations for X509_STORE. X509_STORE_load_locations(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
            }

            return true;
        }

        return false;
    } catch (const std::runtime_error& exception) {
        std::throw_with_nested(exception);
    } catch (...) {
        throw false;
    }
}
