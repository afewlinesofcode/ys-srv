/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-07-29
 * \brief  Basic worker class source file.
 */

#ifndef YS_ASIO_BASIC_WORKER_TCC
#define YS_ASIO_BASIC_WORKER_TCC

#include "basic_worker.h"

namespace ys
{
namespace asio
{

/*!
 * Get a reference to the correctly casted worker object.
 * \return
 */
template<class Worker, class Buffer>
Worker&
basic_worker<Worker, Buffer>::casted()
{
    return static_cast<Worker&>(*this);
}

/*!
 * Get a const-reference to the correctly casted worker object.
 * \return
 */
template<class Worker, class Buffer>
Worker const&
basic_worker<Worker, Buffer>::casted() const
{
    return static_cast<Worker const&>(*this);
}

/*!
 * Receive new TCP connection socket.
 * \param socket_ Accepted socket of a new connection.
 */
template<class Worker, class Buffer>
void
basic_worker<Worker, Buffer>::on_tcp_socket(boost::asio::ip::tcp::socket socket_)
{
    auto c = register_tcp_connection(
                 std::make_shared<tcp_connection_type>(std::move(socket_)));

    c->read();
}

/*!
 * Handle shutdown request.
 */
template<class Worker, class Buffer>
void
basic_worker<Worker, Buffer>::on_shutdown()
{
    for (tcp_connection_ptr c: tcp_connections_)
    {
        unregister_tcp_connection(c);
    }
}

/*!
 * Subscribe for the connection events and add it to the register.
 * \param c Connection pointer.
 */
template<class Worker, class Buffer>
typename basic_worker<Worker, Buffer>::tcp_connection_ptr
basic_worker<Worker, Buffer>::register_tcp_connection(tcp_connection_ptr c)
{
    using namespace boost::placeholders;

    tcp_connections_.insert(c);

    c->on_read(boost::bind(&Worker::on_tcp_connection_read, &casted(),
                           _1, _2));

    c->on_error(boost::bind(&Worker::on_tcp_connection_error, &casted(),
                            _1, _2));

    return c;
}

/*!
 * Unsubscribe from the connection events and remove it from the register.
 * \param c Connection pointer.
 */
template<class Worker, class Buffer>
void
basic_worker<Worker, Buffer>::unregister_tcp_connection(tcp_connection_ptr c)
{
    c->close();

    tcp_connections_.erase(c);
}

} // namespace asio
} // namespace ys

#endif // YS_ASIO_BASIC_WORKER_TCC

