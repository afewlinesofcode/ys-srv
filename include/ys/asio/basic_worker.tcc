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
    auto c = register_connection(
                 std::make_shared<tcp_conn_type>(std::move(socket_)));

    c->read();
}

/*!
 * Handle shutdown request.
 */
template<class Worker, class Buffer>
void
basic_worker<Worker, Buffer>::on_shutdown()
{
    for (tcp_conn_ptr c: tcp_connections_)
    {
        unregister_connection(c);
    }
}

/*!
 * Set handler for TCP connection registered signal.
 * \param cb
 */
template<class Worker, class Buffer>
void
basic_worker<Worker, Buffer>::on_connection_registered(
    typename on_tcp_conn_reg_type::slot_type const& cb)
{
    on_tcp_conn_reg_signal_.connect(cb);
}

/*!
 * Set handler for TCP connection unregistered signal.
 * \param cb
 */
template<class Worker, class Buffer>
void
basic_worker<Worker, Buffer>::on_connection_unregistered(
    typename on_tcp_conn_unreg_type::slot_type const& cb)
{
    on_tcp_conn_unreg_signal_.connect(cb);
}

/*!
 * Subscribe for the connection events and add it to the register.
 * \param c Connection pointer.
 */
template<class Worker, class Buffer>
typename basic_worker<Worker, Buffer>::tcp_conn_ptr
basic_worker<Worker, Buffer>::register_connection(tcp_conn_ptr c)
{
    using namespace boost::placeholders;

    tcp_connections_.insert(c);

    on_tcp_conn_reg_signal_(this->shared_from_this(), c);

    return c;
}

/*!
 * Unsubscribe from the connection events and remove it from the register.
 * \param c Connection pointer.
 */
template<class Worker, class Buffer>
void
basic_worker<Worker, Buffer>::unregister_connection(tcp_conn_ptr c)
{
    c->close();

    tcp_connections_.erase(c);

    on_tcp_conn_unreg_signal_(this->shared_from_this(), c);
}

} // namespace asio
} // namespace ys

#endif // YS_ASIO_BASIC_WORKER_TCC

