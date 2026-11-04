#include "sbcomp.hxx"
#include "scanner.hxx"
#include "token.hxx"

// Override of SbModule::SetSource32() - compiled into the test binary so
// the linker picks our version instead of libsb.so's buggy one (which has
// the TokenLabelInfo buffer overflow bug causing heap corruption).

void SbModule::SetSource32( const ::rtl::OUString& r )
{
    aOUSource = r;
    StartDefinitions();
    SbiTokenizer aTok( r );
    while( !aTok.IsEof() )
    {
        SbiToken eEndTok = NIL;

        // Suchen nach SUB oder FUNCTION
        SbiToken eLastTok = NIL;
        while( !aTok.IsEof() )
        {
            SbiToken eCurTok = aTok.Next();
            if( eLastTok != DECLARE )
            {
                if( eCurTok == SUB )
                {
                    eEndTok = ENDSUB; break;
                }
                if( eCurTok == FUNCTION )
                {
                    eEndTok = ENDFUNC; break;
                }
                if( eCurTok == PROPERTY )
                {
                    eEndTok = ENDPROPERTY; break;
                }
                if( eCurTok == OPTION )
                {
                    eCurTok = aTok.Next();
                    if( eCurTok == COMPATIBLE )
                        aTok.SetCompatible( true );
                    else if ( ( eCurTok == VBASUPPORT ) && ( aTok.Next() == NUMBER ) )
                    {
                        sal_Bool bIsVBA = ( aTok.GetDbl()== 1 );
                        SetVBACompat( bIsVBA );
                        aTok.SetCompatible( bIsVBA );
                    }
                }
            }
            eLastTok = eCurTok;
        }
        // Definition der Methode
        SbMethod* pMeth = NULL;
        if( eEndTok != NIL )
        {
            sal_uInt16 nLine1 = aTok.GetLine();
            if( aTok.Next() == SYMBOL )
            {
                String aName_( aTok.GetSym() );
                SbxDataType t = aTok.GetType();
                if( t == SbxVARIANT && eEndTok == ENDSUB )
                    t = SbxVOID;
                pMeth = GetMethod( aName_, t );
                pMeth->nLine1 = pMeth->nLine2 = nLine1;
                // Die Methode ist erst mal GUELTIG
                pMeth->bInvalid = sal_False;
            }
            else
                eEndTok = NIL;
        }
        // Skip bis END SUB/END FUNCTION
        if( eEndTok != NIL )
        {
            while( !aTok.IsEof() )
            {
                if( aTok.Next() == eEndTok )
                {
                    pMeth->nLine2 = aTok.GetLine();
                    break;
                }
            }
            if( aTok.IsEof() )
                pMeth->nLine2 = aTok.GetLine();
        }
    }
    EndDefinitions( sal_True );
}
