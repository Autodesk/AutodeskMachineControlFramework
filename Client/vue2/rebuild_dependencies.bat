del package.json 
del package-lock.json 

copy package_template.json package.json

echo "Updating Production Dependencies"
call npm install @mdi/font --save
call npm install asmcrypto-lite@1.x.x --save
call npm install axios@0.24.x --save 
call npm install luxon@3.x.x --save 
call npm install roboto-fontface --save 
call npm install three --save 
call npm install three-svg-loader --save
call npm install vue@2.x.x --save 
call npm install vuetify@2.x.x --save

echo "Updating Develop Dependencies"
call npm install @vue/cli-plugin-babel --save-dev
call npm install @vue/cli-plugin-eslint --save-dev
call npm install @vue/cli-service --save-dev
call npm install sass --save-dev
call npm install sass-loader --save-dev
call npm install vue-cli-plugin-vuetify --save-dev
call npm install vue-template-compiler --save-dev 
call npm install eslint --save-dev
call npm install vuetify-loader --save-dev
call npm install eslint-plugin-vue --save-dev
call npm install @babel/eslint-parser --save-dev