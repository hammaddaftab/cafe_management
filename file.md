# Project Basic Framework

## Products Management Page `/admin/products`
### Overiew
This is only for the admin to add/ update and remove products
It will require a hardcoded password (hardcoded because
we are only making it for a single cafe for the time being)

Visiting this page without authentication will redirect the user
to `/admin/auth` where the admin will enter their credentials.
More details on `/admin/auth` section.



### TODO `/admin/products/view`
This will be the view page to see all the products. 
The admin can edit or delete a product by cliking on the 
edit button provided right to each element which
will redirect the admin to `/admin/products/edit?id=<id>`



### TODO `/admin/products/add`
- [ ] Page static HTML Design
- [ ] Server Implementation
- [ ] Database Integraion

#### Pertaining to Frontend
The request from frontend will include all the required fields
in a `POST` form request at this route to add the product.
Allow sending multiple items at once but each input should have 
a unique `name` attribute. Use `js` to append more and more
`input` groups for each new product on pressing a btn like 
`Add Product` and a final `Save` button which sends the request.



### TODO `/admin/products/delete`
For deleting a product, the request will be sent to 
`/admin/products/delete?id=<replace_with_id>` and the server will 
handle the rest.

- [ ] Server Implementation
- [ ] Database Integration



### TODO for `/admin/auth`
- [ ] Page static HTML Design
- [ ] Server Implementation

Authenticating an admin is done through a certain pre-defined
session cookie, whoever attains this cookie is considered the
admin.When the user enters the correct credentials, the server
will send in response this cookie shifting the roles from
user to admin.

#### Pertaining to Frontend
Takes username and password form the user and then
submit the the form with `POST` method
to the server at `/admin/auth`