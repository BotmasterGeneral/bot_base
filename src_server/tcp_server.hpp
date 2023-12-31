#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP


/*****************************************************************************/
/*                                                                     Guest */
/*****************************************************************************/
    class Guest
    {
    public:
        struct GuestMembers;
        GuestMembers *pGuestMembers;

        unsigned    guestid;
        bool        keepalive = false;
        bool        deflateSupported = false;

        explicit Guest(void *ioCtx);

        Guest(const Guest&) = delete;
        Guest& operator=(const Guest&) = delete;

        Guest(Guest&&);
        Guest& operator=(Guest&&) = delete;

        void swap(Guest&);
        
        ~Guest();
    };

    
/*****************************************************************************/
/*                                                      'homepage' container */
/*****************************************************************************/
    class Page_base
    {
    public:
        enum Options : unsigned { CHUNKED=1 };
        
    protected:
        unsigned options = 0;
        bool sendHeader = true;
        bool chunksDone = false;

        struct PageMembers;
        PageMembers *pPageMembers;

        enum class Mime { HTM,HTML,CSS,JS,JSON,ICON,PNG,FLV,JPG,JPEG,GIF,SVG };
    
    protected:
        void enable(Options);
        
        void disable(Options);
        
        virtual void placeChunk(void *) = 0;

        virtual Mime getType() const = 0;
        
    public:
        Page_base();
        
        Page_base(const Page_base&) = delete;
        Page_base& operator=(const Page_base&) = delete;

        virtual bool findRequestedPage(const char *, int) = 0; // todo should return status, not bool

        virtual void submitBody(const char *, int) = 0;
        
        virtual bool found() const = 0;

        bool maxLenExceeded(int) const {return false;}//todo

        bool delivered() const;

        void buffers(void *, const Guest&, const int httpStatus);

        virtual void reset() = 0;
        
        virtual Page_base *createNew() const = 0;

        virtual ~Page_base();
    };

    
/*****************************************************************************/
/*****************************************************************************/
    class TcpServer
    {
    public:
        static constexpr int max_concurrent_users = 1000;
        static constexpr int max_concurrent_connections = max_concurrent_users * 6;

        struct TcpServerMembers;
        
    protected:
        TcpServerMembers *pTcpServerMembers;

        Page_base *page_base;

    private:
        void doAccept();
        
    public:
        TcpServer(void *global, Page_base *page, const char *address, const unsigned short port);
        
        TcpServer(const TcpServer&) = delete;
        TcpServer& operator=(const TcpServer&) = delete;

        virtual void go();

        void shutdown();
        
        virtual ~TcpServer();
    };


    class TcpServerSSL : public TcpServer
    {
    private:
        struct SSL_Container;
        SSL_Container *pSSL_Container;
        
        void doAcceptSSL();
        
    public:
        TcpServerSSL( void *global
                    , Page_base *page
                    , const char *address
                    , const unsigned short port
                    , const char *chain_file, const char *key_file, const char *dh_file
                    );

        void go() override;
        
        void refreshSSL(const char *chain_file, const char *key_file, const char *dh_file);
        
        ~TcpServerSSL();
    };


#else
  #error "double include"
#endif // TCP_SERVER_HPP


