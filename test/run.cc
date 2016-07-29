/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-07-29
 * \brief  Run simple echo server listening on localhost:12345 with two workers.
 */

#include <array>
#include <vector>

#include <ys/asio/simple_server.h>
#include <ys/asio/basic_worker.h>
#include <ys/logger.h>

/*!
 * A worker class.
 */
class worker
    : public ys::asio::basic_worker<worker, std::array<char, 1024>>
{
public:
    /*!
     * Process data arrived on connection.
     * Write received data back to the connection.
     * \param c The connection pointer.
     * \param s Size of the arrived data.
     */
    void
    on_tcp_connection_read(tcp_connection_ptr c, std::size_t s)
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
    on_tcp_connection_error(tcp_connection_ptr c,
                            boost::system::error_code const& ec)
    {
        YS_LOG(error) << "connection error: "
                      << ec << " [" << ec.message() << "]";

        /*
         * Unregister connection on any error.
         */
        unregister_tcp_connection(c);
    }
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

