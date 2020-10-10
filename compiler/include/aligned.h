class alignedText
{
    mutable std::ostream* myOwner;
    mutable std::ostream::fmtflags myFlags;
    int myWidth;
public:
    alignedText( int width ) : myOwner(nullptr), myWidth( width ) {}
    ~alignedText()
    {
        if ( myOwner != nullptr ) {
            myOwner->flags( myFlags );
        }
    }
    void set( std::ostream& dest ) const
    {
        if ( myOwner == nullptr ) {
            myOwner = &dest;
            myFlags = myOwner->flags();
        }
        dest.setf( std::ios_base::left, std::ios_base::adjustfield );
        dest.width( myWidth );
    }

    friend std::ostream& operator<<( std::ostream& dest, alignedText const& manip )
    {
        manip.set( dest );
        return dest;
    }
};