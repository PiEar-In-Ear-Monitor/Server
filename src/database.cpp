//******************************************************************************
// Copyright © 2021  Alexander O'Connor
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//******************************************************************************

#include "database.h"
#include <sqlite3.h>
#include <stdio.h>
#include <fstream>

bool database::check_for_error() {
    if (this->last_return != SQLITE_OK) {
        perror(sqlite3_errmsg(this->db));
        return true;
    }
    return false;
}

void database::create_from_scratch() {
    this->last_return = sqlite3_open_v2(this->default_file, &this->db, SQLITE_OPEN_READWRITE, NULL);
    sqlite3_close(this->db);
    if (!this->check_for_error()) {
        this->reset_database();
    }
    this->last_return = sqlite3_open_v2(this->label_file, &this->db, SQLITE_OPEN_READWRITE, NULL);
    if (this->check_for_error()) {
        exit(DATABASE_CANNOT_WRITE);
    }
    //!< @todo Create database
}

database::database() {
    this->label_file = "PiEar_Labels.db";
    this->default_file = "PiEar_Labels_default.db";
    this->last_return = sqlite3_open_v2(this->label_file, &this->db, SQLITE_OPEN_READWRITE, NULL);
    if (this->check_for_error()) {
        this->create_from_scratch();
    }
}

database::~database() {
    sqlite3_close(this->db);
    this->check_for_error();
    delete this->error_message;
}

void database::reset_database() {
    std::ifstream  src(this->default_file, std::ios::binary);
    std::ofstream dst;
    if (src.good()) {
        dst.open(this->label_file,   std::ios::binary);
    } else {
        src.close();
        src.open(this->label_file, std::ios::binary);
        dst.open(this->default_file,   std::ios::binary);
    }
    dst << src.rdbuf();
}

void database::change_label(int index, char *new_label) {

}

char* database::get_label(int index) {
    std::string sql = "SELECT label FROM stream_labels WHERE " + std::to_string(index);
}
