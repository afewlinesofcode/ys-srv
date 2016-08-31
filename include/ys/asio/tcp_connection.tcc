/*!
 * \file
 * \date   2016-07-01
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \brief  A TCP connection class source file.
 */

#ifndef YS_ASIO_TCP_CONNECTION_TCC
#define YS_ASIO_TCP_CONNECTION_TCC

#include "tcp_connection.h"

namespace ys
{
namespace asio
{

/*!
 * Constructor.
 * \param s Accepted socket.
 */
template<typename Buffer>
tcp_connection<Buffer>::tcp_connection(boost::asio::ip::tcp::socket s) :
    socket_ { std::move(s) }
{
}

/*!
 * Whether the connection socket is open.
 * \return
 */
template<typename Buffer>
bool
tcp_connection<Buffer>::is_open()
{
    return socket_.is_open();
}

/*!
 * Start reading the connection.
 */
template<typename Buffer>
void
tcp_connection<Buffer>::read()
{
    do_read();
}

/*!
 * Write to the connection.
 */
template<typename Buffer>
template<typename WriteBuffer, typename WriteHandler>
void
tcp_connection<Buffer>::write(WriteBuffer const& buffer,
                              WriteHandler const& handler)
{
    socket_.async_send(boost::asio::buffer(buffer), handler);
}

/*!
 * Close the connection.
 */
template<typename Buffer>
void
tcp_connection<Buffer>::close()
{
    socket_.close();
}

/*!
 * Get a const-reference to the connection buffer.
 * \return
 */
template<typename Buffer>
typename tcp_connection<Buffer>::buffer_type const&
tcp_connection<Buffer>::buffer() const
{
    return data_buffer_;
}

/*!
 * Get a const reference to the underlying boost socket.
 * \return
 */
template<typename Buffer>
boost::asio::ip::tcp::socket const&
tcp_connection<Buffer>::socket() const
{
    return socket_;
}

/*!
 * Set a callback for new data processing.
 * \param cb
 */
template<typename Buffer>
void
tcp_connection<Buffer>::on_data(typename on_data_type::slot_type const& cb)
{
    on_data_signal_.connect(cb);
}

/*!
 * Set a callback for error handling.
 * \param cb
 */
template<typename Buffer>
void
tcp_connection<Buffer>::on_error(typename on_error_type::slot_type const& cb)
{
    on_error_signal_.connect(cb);
}

/*!
 * Asynchronously read from the socket.
 */
template<typename Buffer>
void
tcp_connection<Buffer>::do_read()
{
    socket_.async_read_some(boost::asio::buffer(data_buffer_),
                            [this](boost::system::error_code const& ec,
                                   std::size_t bytes_transferred)
    {
        if (!ec)
        {
            /*
             * No error. Triggering new data signal.
             */
            on_data_signal_(this->shared_from_this(), bytes_transferred);
        }
        else
        {
            /*
             * Error! Triggering error signal.
             */
            on_error_signal_(this->shared_from_this(), ec);
        }

        /*
         * Go on reading if the socket is still open.
         */
        if (socket_.is_open())
        {
            do_read();
        }
    });
}

} /* namespace asio */
} /* namespace ys */

#endif // YS_ASIO_TCP_CONNECTION_TCC

