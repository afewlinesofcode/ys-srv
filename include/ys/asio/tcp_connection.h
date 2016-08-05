/*!
 * \file
 * \date   2016-07-01
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \brief  A TCP connection class header file.
 */

#ifndef YS_ASIO_TCP_CONNECTION_H
#define YS_ASIO_TCP_CONNECTION_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

namespace ys
{
namespace asio
{

template<typename Buffer>
class tcp_connection:
    public std::enable_shared_from_this<tcp_connection<Buffer>>
{
public:
    /*!
     * A TCP connection pointer typedef.
     */
    using ptr = std::shared_ptr<tcp_connection>;

    /*!
     * A buffer typedef.
     */
    using buffer_type = Buffer;

    /*!
     * A new data callback typedef.
     */
    using on_data_type =
        boost::signals2::signal<void (ptr, std::size_t)>;

    /*!
     * An error callback typedef.
     */
    using on_error_type =
        boost::signals2::signal<void (ptr, boost::system::error_code const&)>;

    /*!
     * Constructor.
     * \param s Accepted socket.
     */
    tcp_connection(boost::asio::ip::tcp::socket s);

    /*!
     * Copy is prohibited.
     * \param
     */
    tcp_connection(tcp_connection const&) = delete;

    /*!
     * Copy is prohibited.
     * \param
     * \return
     */
    tcp_connection&
    operator=(tcp_connection const&) = delete;

    /*!
     * Whether socket is open.
     * \return
     */
    bool
    is_open();

    /*!
     * Start reading the connection.
     */
    void
    read();

    /*!
     * Write to the connection.
     */
    template<typename WriteBuffer, typename WriteHandler>
    void
    write(WriteBuffer const& buffer, WriteHandler const& handler);

    /*!
     * Close the connection.
     */
    void
    close();

    /*!
     * Get a const-reference to the connection buffer.
     * \return
     */
    buffer_type const&
    buffer() const;

    /*!
     * Get a const reference to the underlying boost socket.
     * \return
     */
    boost::asio::ip::tcp::socket const&
    socket();

    /*!
     * Set a callback for new data processing.
     * \param cb
     */
    void
    on_data(typename on_data_type::slot_type const& cb);

    /*!
     * Set a callback for error handling.
     * \param cb
     */
    void
    on_error(typename on_error_type::slot_type const& cb);

private:
    /*!
     * A connection socket.
     */
    boost::asio::ip::tcp::socket socket_;

    /*!
     * A buffer with data.
     */
    buffer_type data_buffer_;

    /*!
     * A new data signal.
     */
    on_data_type on_data_signal_;

    /*!
     * An error signal.
     */
    on_error_type on_error_signal_;

    /*!
     * Asynchronously read from the socket.
     */
    void
    do_read();
};

} /* namespace asio */
} /* namespace ys */

#include "tcp_connection.tcc"

#endif /* YS_ASIO_TCP_CONNECTION_H */

