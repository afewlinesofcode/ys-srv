/*!
 * \file
 * \date   2015-08-21
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \brief  A logger class header file.
 */

#ifndef YS_LOGGER_H
#define YS_LOGGER_H

#define BOOST_LOG_DYN_LINK 1

#define YS_LOG(severity)\
	BOOST_LOG_SEV(ys::logger::get_logger(), boost::log::trivial::severity)

#include <boost/log/trivial.hpp>

namespace ys
{

/*!
 * \brief A logger class.
 * \details Used by YS_LOG macros.
 *          Example:
 *              YS_LOG(debug) << "A test log message";
 */
class logger
{
public:
    /*!
     * A severity type typedef.
     */
    using severity_type =
        boost::log::trivial::severity_level;

    /*!
     * A logger type typedef.
     */
    using logger_type =
        boost::log::sources::severity_logger<severity_type>;

    /*!
     * Get a logger object.
     * \return
     */
    static
    logger_type&
    get_logger();

private:
    /*!
     * A logger object.
     */
    static logger_type lg;
};

} // namespace ys

#endif /* YS_LOGGER_H */

