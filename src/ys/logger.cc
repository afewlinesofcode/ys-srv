/*!
 * \file
 * \date   2015-08-21
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \brief  A logger class source file.
 */

#include <ys/logger.h>

namespace ys
{

/*!
 * A logger object.
 */
logger::logger_type logger::lg;

/*!
 * Get a logger object.
 * \return
 */
logger::logger_type&
logger::get_logger()
{
    return lg;
}

} // namespace ys

