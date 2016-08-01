/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-07-29
 * \brief  Simple echo server listening on localhost:12345 with two workers.
 */

#include <array>
#include <vector>

#include <ys/asio/simple_server.h>
#include <ys/asio/basic_worker.h>
#include <ys/logger.h>

using buffer_type = std::array<char, 1024>;

/*!
 * A worker class.
 */
class worker
    : public ys::asio::basic_worker<worker, buffer_type>
{
public:
    /*!
     * \brief Constructor.
     * \details Initialize handlers for connection events.
     */
    worker() :
        on_tcp_conn_read_
        (
            boost::bind(&worker::on_conn_read, this, _1, _2)
        ),
        on_tcp_conn_error_
        (
            boost::bind(&worker::on_conn_error, this, _1, _2)
        )
    {
        on_connection_registered(boost::bind(
                                     &worker::on_conn_reg,
                                     this,
                                     _1, _2));

        on_connection_unregistered(boost::bind(
                                       &worker::on_conn_unreg,
                                       this,
                                       _1, _2));
    }

    /*!
     * Handler called after a new connection was registered.
     * \param w Worker holding connection.
     * \param c The connection.
     */
    void
    on_conn_reg(ptr w, tcp_conn_ptr c)
    {
        using namespace boost::placeholders;

        c->on_read(on_tcp_conn_read_);
        c->on_error(on_tcp_conn_error_);
    }

    /*!
     * Handler called after a connection was unregistered.
     * \param w Worker holding connection.
     * \param c The connection.
     */
    void
    on_conn_unreg(ptr w, tcp_conn_ptr c)
    {
    }

    /*!
     * Process data arrived on connection.
     * Write received data back to the connection.
     * \param c The connection pointer.
     * \param s Size of the arrived data.
     */
    void
    on_conn_read(tcp_conn_ptr c, std::size_t s)
    {
        char const* data = c->buffer().data();
        std::vector<char> vec { data, data + s };

        c->write(
            boost::asio::buffer(vec),
            [](boost::system::error_code const& ec, std::size_t s)
        {
            if (!ec)
            {
                YS_LOG(debug) << "written " << s << " bytes";
            }
            else
            {
                YS_LOG(error) << "error on write: " << ec.message();
            }
        });
    }

    /*!
     * Handle error on connection.
     * \param c The connection pointer.
     * \param ec The error code.
     */
    void
    on_conn_error(tcp_conn_ptr c,
                  boost::system::error_code const& ec)
    {
        YS_LOG(error) << "connection error: "
                      << ec << " [" << ec.message() << "]";

        /*
         * Unregister connection on any error.
         */
        unregister_connection(c);
    }

private:
    /*!
     * Handler for TCP connection data event.
     */
    tcp_conn_type::on_read_type::slot_type on_tcp_conn_read_;

    /*!
     * Handler for TCP connection error event.
     */
    tcp_conn_type::on_error_type::slot_type on_tcp_conn_error_;
};

/*!
 * Main function.
 * \param argc Args num.
 * \param argv Args vals.
 */
int
main(int argc, char* argv[])
{
    YS_LOG(debug) << "ECHO-server started";

    /*
     * Create server object with two workers.
     */
    ys::asio::simple_server<worker> srv({ 2 });

    /*
     * Set address `127.0.0.1:12345` for listening.
     */
    srv.tcp("127.0.0.1", "12345");

    /*
     * Start the server.
     */
    srv.run();

    YS_LOG(debug) << "ECHO-server finished";

    return 0;
}

