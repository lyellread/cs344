keygen: provides keygen capabilities. Use: `keygen n` where n is the int length of key

Program Structure

note: lower programs call some functions from their connected higher programs
    (i.e. otp_core.c contains funcions used both in otp.c and otp_d.c)

    otp_core.c
        ||
        |'------ otp.c
        |          ||
        |          |'----- otp_dec.c
        '- otp_d.c '------ otp_enc.c
              ||
              |'----- otp_enc_d.c
              '------ otp_dec_d.c
