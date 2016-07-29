/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-07-29
 * \brief  ys::asio base error class header file.
 */

#include <stdexcept>

namespace ys
{
namespace asio
{

/*!
 * ys::asio base error class.
 */
class error: public std::runtime_error
{
public:
    /*!
     * Constructor.
     * \param msg Error message.
     */
    error(std::string const& msg)
        : runtime_error(msg)
    {
    }
};

} // namespace asio
} // namespace ys
