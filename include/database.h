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


#ifndef PIEAR_SERVER_DATABASE_H
#define PIEAR_SERVER_DATABASE_H

#define DATABASE_CANNOT_WRITE 400

#include <sqlite3.h>

class database {
private:
    sqlite3 *db;         //!< Database pointer
    int last_return;     //!< Last return from database operation
    char *error_message; //!< Error message
    const char *label_file;    //!< Name of label database
    const char *default_file;   //!< Name of backup database
    /**
     * Checks `this->last_return` to see if there is an error.
     * @return success
     */
    bool check_for_error();

    void create_from_scratch();
public:
    /**
     * Sets up the database.
     *
     * If it doesn't exist, it pulls from the default.
     * On the first load of the program, it is expected
     * that the admin will create a default, which will
     * then be saved when it is "reset"
     */
    database();

    /**
     * Destroys object and closes connection
     */
    ~database();

    /**
     * Reset's the label database.
     *
     * If a default doesn't exist, it assumes that what
     * is current is supposed to be the default.
     */
    void reset_database();

    /**
     * Changes label at `index` to `new_label`
     *
     * @param index Index to be changed
     * @param new_label New label for item at index `index`
     */
    void change_label(int index, char *new_label);

    /**
     * Gets label at index `index`
     * @param index Index to be retrieved
     * @return label
     */
    char* get_label(int index);
};

#endif //PIEAR_SERVER_DATABASE_H
