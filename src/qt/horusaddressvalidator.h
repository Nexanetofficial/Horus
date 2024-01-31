// Copyright (c) 2011-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HORUS_QT_HORUSADDRESSVALIDATOR_H
#define HORUS_QT_HORUSADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class HorusAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit HorusAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

/** Horus address widget validator, checks for a valid horus address.
 */
class HorusAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit HorusAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

#endif // HORUS_QT_HORUSADDRESSVALIDATOR_H
