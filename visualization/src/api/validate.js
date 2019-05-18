export function usernameValidate(username)
{
	if(!username)
		return 'At least 4 characters';
	if(username.length < 4)
		return 'At least 4 characters';
	if(username.length > 20)
		return 'At most 20 characters';
	if(!(/^[A-Za-z0-9_]+$/.test(username)))
		return 'Allowed characters: A-Za-z0-9_';
	return true;
}

export function passwordValidate(password)
{
	if(!password)
		return 'At least 4 characters';;
	if(password.length < 4)
		return 'At least 4 characters';
	if(password.length > 20)
		return 'At most 20 characters';
	if(!(/^[\x21-\x7E]+$/.test(password)))
		return 'Allowed characters: 0x21-0x7E in ASCII';
	return true;
}

export function emailValidate(email)
{
	if(/^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/.test(email))
		return true;
	return 'Invalid Email';
}

export function IPValidate(ip)
{
	if(/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ip))
		return true;
	return 'Invalid IP Address';
}

