%option noyywrap

%option reentrant noyywrap nodefault yylineno 
%option extra-type="AssemblerContext*"

%{

#include <assembler_context.h>
#include <mips_instr.h>
#include <token.h>
#include <cstdlib>

%}


ws [ \t\r,()]+

digit ([-+])?[0-9]+
alphanum [a-zA-Z0-9]+
label {alphanum}

%%

{ws}    {}

"$zero" {
          yyextra->add_token(RegIdx::ZERO);
        }
        
"$t0"   {
          yyextra->add_token(RegIdx::T0);
        }
        
"$t1"   {
          yyextra->add_token(RegIdx::T1);
        }
        
"$t2"   {
          yyextra->add_token(RegIdx::T2);
        }
        
"$t3"   {
          yyextra->add_token(RegIdx::T3);
        }
        
"$t4"   {
          yyextra->add_token(RegIdx::T4);
        }    

"$t5"   {
          yyextra->add_token(RegIdx::T5);
        }    
        
"$t6"   {
          yyextra->add_token(RegIdx::T6);
        }    

"$t7"   {
          yyextra->add_token(RegIdx::T7);
        }    

"$s0"   {
          yyextra->add_token(RegIdx::S0);
        }    

"$s1"   {
          yyextra->add_token(RegIdx::S1);
        }    

"$s2"   {
          yyextra->add_token(RegIdx::S2);
        }    
       
"$s3"   {
          yyextra->add_token(RegIdx::S3);
        }    
        
"$io"   {
          yyextra->add_token(RegIdx::IO);
        }    

"$bp"   {
          yyextra->add_token(RegIdx::BP);
        }    
        
"$sp"   {
          yyextra->add_token(RegIdx::SP);
        }    

"nor"   {
          yyextra->add_token(OpCode::NOR);
        }

"srl"   {
          yyextra->add_token(OpCode::SRL);
        }
        
"and"   {
          yyextra->add_token(OpCode::AND);
        }
        
"sll"   {
          yyextra->add_token(OpCode::SLL);
        }
        
"bneq"  {
          yyextra->add_token(OpCode::BNEQ);
        }
 
"j"     {
          yyextra->add_token(OpCode::J);
        }
        
"andi"  {
          yyextra->add_token(OpCode::ANDI);
        }
        
"subi"  {
          yyextra->add_token(OpCode::SUBI);
        }
        
"lw"    {
          yyextra->add_token(OpCode::LW);
        }
       
"sw"    {
          yyextra->add_token(OpCode::SW);
        }
        
"sub"   {
          yyextra->add_token(OpCode::SUB);
        }
        
"addi"  {
          yyextra->add_token(OpCode::ADDI);
        }
        
"or"    {
          yyextra->add_token(OpCode::OR);
        }
        
"beq"   {
          yyextra->add_token(OpCode::BEQ);
        }
        
"ori"   {
          yyextra->add_token(OpCode::ORI);
        }
        
"add"   {
          yyextra->add_token(OpCode::ADD);
        }        

{digit} {
            int x;
          yyextra->add_token(x = std::atoi(yytext));
            std::cerr << x << std::endl;
        }


{label}: {
              yyextra->add_token(
                  yyextra->get_label(std::string_view(yytext, yytext + yyleng - 1)));
         }

{label} {
          yyextra->add_token(
              yyextra->get_label(yytext));
        }

"\n"    {
          yyextra->new_line(yylineno);
        }


        
%%


void AssemblerContext::init_scanner()
{
    yylex_init_extra(this, &scanner_);
    yyset_in(in_file_, scanner_);
    yylex(scanner_);
}

void AssemblerContext::finish_scanner(){
    auto line = yyget_lineno(scanner_);
    yylex_destroy(scanner_);
}

